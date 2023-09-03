## OVERVIEW:
In the first phase of the project, we're mainly focused on handling RA (Relational Algebra) matrices. This involves storing them, getting them back when needed, and doing some basic operations on them. When we work with matrices and tables together, our main challenge is dealing with restrictions on how big the columns can be and handling situations where there are no column labels. We're also working on making the process of getting data from matrices and writing data to them as efficient as possible, taking into account the special characteristics of matrices.


## PAGE DESIGN:
We refrain from unnecessary duplication of effort by leveraging the existing page design, specifically the Page(page.h) class. However, we enhance it by introducing an additional constructor. This new constructor allows for the dynamic specification of the page's column count, as it is not a fixed value. Furthermore, the page fetch constructor incorporates a **isMatrix** flag. This flag enables the retrieval of essential metadata from the relevant catalog before proceeding with the page reading operation.


We maintain consistency by employing the same cursor and buffer manager for row iteration and cache management. However, certain refinements have been made:
1. **Cursor**: The constructor now includes the **isMatrix** flag, which guides it in selecting the appropriate catalog when querying for the next page.

2. **Buffer Manager**: We have introduced some modifications, including an overloaded 'writePage' method that accommodates a specific column count. Additionally, a new **deleteFromPool** method has been added to invalidate pages after performing in-place rename or transpose operations. The 'insertIntoPool' method now incorporates an additional parameter **isMatrix** to be passed on to the Page constructor.

3. **Parsed Query**: We have incorporated an 'isMatrix' flag within the ParsedQuery, which enables the reuse of semantic parsing and execution blocks for actions such as load, print, rename, and export.


## OTHER MAJOR CHANGELOGS:
The **isMatrix** flag serves a dual purpose by enabling us to employ the same syntactic and semantic parser for operations such as LOAD, PRINT, EXPORT, and RENAME, as previously defined for tables. These parsers perform semantic checks adeptly, gracefully handling errors that may arise due to non-existent matrices or invalid arguments.


### Optimization, \# of accesses
To achieve optimal iteration performance and streamline lookups, we proactively calculate two crucial parameters: the total number of page blocks and the number of page blocks allocated per row. Furthermore, we capitalize on the advantageous characteristic that the matrix is **square** in shape. During the print operation, we judiciously interrupt the iteration process and exclusively write the real elements to the Page blocks, omitting any unnecessary padding. This approach enhances both efficiency and storage utilization.

We create pages that are about 1KB in size and can hold up to 16*16=256 integers, whether they're positive or negative. We claculated this out as follow : sqrt(PAZ_SIZE_IN_BYTES/sizeof(int)) = sqrt(1024/sizeof(int)).
We've kept the same page size and the buffer pool size(= 2), consistent with our original table storage design. The number of pages is given by **blockCount** and interestingly, this number is always a perfect square, like 4, 9, 16, and so on.


### Assumptions
- We operate under the assumption that the pages located in the data/temp directory are secure and protected, reserved exclusively for use by the application. Consequently, we do not find it necessary to perform consistency checks on the catalog.

- Our assumptions include the availability of ample heap memory to accommodate a lengthy catalog, and we rely on the buffer manager's capability to maintain a deque with a minimum size of 2 pages.

- We presume that the provided CSV files contain valid square matrices and that the characters within can be successfully converted to integers without causing overflow issues within the range of -2^31-1 to +2^31. Additionally, we anticipate that mathematical operations will not result in subtraction overflows.

- We make the assumption that the .csv files will remain accessible after loading and exporting matrices, unless explicitly removed. Failure to adhere to this assumption may result in permanent loss of the matrices.

During the print and export operations, pages are read one after the other as the cursor progresses to the end of the current page. This results in accessing a number of cursor positions equal to ceil(size/16)*size.

When it comes to operations like transpose, checksymmetry, and compute, the number of page accesses matches the blockCount. However, we've optimized the order of access to minimize the use of buffer memory. For more detailed information on these operations, please refer to further documentation.

LOAD
> To load a matrix, we begin by creating a matrix object, specifying its name and size. We then read the matrix in chunks, each containing a maximum of 'maxRowsperblock' rows, from the corresponding .csv file. After that, we employ the 'writePage' method of the bufferManager, which has been modified to handle this task. This method writes the page blocks to the storage and assigns them indexed names.

> Upon successful loading, the matrix's name matches the file name, and we store the matrix object, along with its metadata, in the matrix catalog. This catalog is organized as an unordered_map, and we index the objects solely by their names.
_Syntax_: ```LOAD MATRIX <FILENAME>```

PRINT
> To print the top-left 20x20 cells of the matrix, we retrieve the necessary number of lines using the cursor. This process takes advantage of the fact that the page blocks were written in a row-major order. Consequently, the submatrix buffers are generated in the same row-major order and then added to the result matrix.
_Syntax_: ```PRINT MATRIX <FILENAME>```

RENAME
> To rename the matrix, we follow the below process:
> 1. Initially, we remove the pages of the matrix from the buffer.
> 2. Then, we proceed to rename the pages associated with the matrix.
> 3. Next, we eliminate the previous matrix entry from the catalog.
> 4. Finally, we update the matrixName and reintroduce the matrix with the new name into the catalog. \
_Syntax_: ```RENAME MATRIX <OLDNAME> <NEWNAME>```

EXPORT
> When exporting, we employ a similar approach as used in printing. Instead of retrieving just 20 lines, we access all the lines in all pages using the cursor. \
_Syntax_: ```EXPORT MATRIX <FILENAME>```

CHECKSYMMETRY
> Our process involves a two-step iteration. Initially, we iterate through the diagonal blocks one by one. Subsequently, we proceed to iterate through the non-diagonal blocks, handling two of them simultaneously. The pageIndex of the second block is calculated based on the first. \
> Symmetry checks are straightforward when dealing with diagonal blocks, as we perform them in a straightforward manner. However, for non-diagonal blocks, we compare corresponding elements. If we encounter any mismatch during this comparison, the function immediately returns FALSE. Only when all the checks are successful does it return TRUE. \
_Syntax_: ```CHECKSYMMETRY <MATRIXNAME>```

TRANSPOSE
> Our approach involves iterating through the blocks within the matrix. When a block is situated on the principal diagonal, we simply transpose its elements. However, if it's a non-diagonal block, we determine the block it should be swapped with to achieve the transpose operation. To efficiently carry out this operation in-place, we calculate the transpositions in pairs of blocks and then write the data back into their respective counterparts. \
> Notably, the memory usage, aside from the blocks themselves, remains limited to the size of 2 pages. This ensures that the entire operation remains in-place. \
_Syntax_: ```TRANSPOSE MATRIX <MATRIXNAME>```

COMPUTE
> We execute a two-step loop process: initially on the diagonal blocks and then on the non-diagonal blocks, much like the approach used in the checksymmetry operation. This method allows us to fetch only the necessary pages for our operations efficiently. \
> Furthermore, we take advantage of the inherent properties of these operations to minimize the number of arithmetic calculations required, focusing only on the lower triangular section of the matrix. \
> To start, we initialize the result matrix with the appropriate dimensions. Then, we utilize the BufferManager to write the necessary pages, indexing them appropriately for the operation. \
_Syntax_: ```COMPUTE <MATRIXNAME>```

## LEARNINGS

All storage methods and object-oriented programming (OOP) structures are carefully designed with the goal of achieving optimal access and adhering to various constraints. An analysis of the function calls and access modifiers provides insight into the underlying rationale for adopting such a design pattern. Throughout the project, we uncovered a variety of considerations related to the read/write access of data blocks, the flow of information between different data structures, exceptional scenarios, and redundancies that were introduced to minimize latency and assumptions about operations.

Furthermore, we noted that dividing the page into blocks that mirror the overall data structure (matrix) significantly improves the efficiency of accessing lines, reducing the need for costly modulo arithmetic operations, especially at scale. As a result, we opted for pages that accommodate square matrices to the maximum extent possible. While this choice does introduce some minor fragmentation at the edges and bottom right of the pages, the time optimization it offers more than compensates for our specific use case.

In addition, we are planning to expand the functionality for unloading matrices to include clearing a specific matrix from the catalogue memory. This enhancement will enable extensive testing of commands and further improve our system's robustness.

## CONTRIBUTIONS
Mayank Gupta ([mayankgupta12321](https://github.com/mayankgupta12321)) - 33.33% \
Ardhendu Banerjee ([ardhendub](https://github.com/ardhendub)) - 33.33% \
Santanu Biswas ([SantanuBiswas99](https://github.com/SantanuBiswas99)) - 33.33%

# Overview
In this phase of the project, we focused on implementing external sorting of rows (of type vector<int>) based on a specified order of columns. We introduced constraints on the number of blocks to read at a time (up to 9 blocks) and allowed for the management of a single output block at a time. Also we have implemented some other commands like ORDER BY, GROUP BY, JOIN using the external sorting.

## SORT BY
We have implemented a sortTable function responsible for sorting data within a table based on user-specified columns and sorting strategies. Here's a breakdown of how this sorting operation works:

### Comparator for Sorting/Priority Queue:
- We begin by defining a custom comparator function, cmpSort, which is used to determine the sorting order for rows based on the specified columns and sorting strategies. This comparator iterates through the selected columns and arranges the data in either ascending (ASC) or descending (DESC) order.
- We also create another custom comparator, cmpPQ, to define the sorting order for the priority queue used during merging. This comparator is similar to the one used in the sorting phase but works with priority queue elements.

### Sorting Phase:
- We initialize a cursor to traverse the table, starting from the first page (page 0).
- For each block (representing a segment of the dataset) in the table, we retrieve the rows contained within that block.
- Using the custom comparator cmpSort, we sort the rows within the block according to the user's specifications.
- The sorted rows are then written back to the buffer with updated information, which is managed by the buffer manager.
- The original page that was read is removed from the buffer pool to free up memory for subsequent operations.
- If there are more blocks to process (i.e., we haven't reached the end of the table), we advance the cursor to the next page and repeat the sorting process.
- After sorting individual blocks, we enter the merge phase to combine and merge these blocks into a final sorted result.
  
### Merge Phase:
- We determine the degree of merge, which is the number of blocks that can be merged at each level. In this case, it's set to `BLOCK_COUNT - 1`.
- We calculate the total number of merge levels required based on the number of blocks in the table.
- We iterate through each merge level, starting from level 0 and progressing to the highest level.
- Within each level, we begin by defining an index and a jump value to identify the blocks to be merged.
- We initialize a priority queue (pq) that will be used to manage the merging process. It uses the cmpPQ comparator for ordering.
- We maintain a pool of cursors, one for each block that is part of the merge, and keep track of the number of records that need to be processed - for each cursor.
- For each segment of blocks (determined by the jump value), we create cursors, add them to the cursor pool, and push the first record from each cursor into the priority queue.
- We process the priority queue, continuously popping the top record and adding it to the merged result. If the number of records to process for a cursor is not zero, we push the next record from that cursor into the priority queue.
- When the merged result reaches a maximum size (defined by maxRowsPerBlock), it is written to a temporary page, and the process continues until all records are merged.
- Once all records are merged at this level, we write the remaining merged records to a temporary page.
- After completing a level of merging, we rename the temporary pages to the table's original page names, ensuring that the sorted data is accessible and organized.
- This sortTable function effectively handles both the sorting and merging phases to provide users with a sorted dataset according to their specified columns and sorting strategies.

## JOIN
- First we parse the query and find relation names, and column names for both the table, and the condition at which we need to join the relations.
- Then we initializes two new temporary table(for both tables) with the same metadata & data as the original table, and sort both the temporary tables based on joining columns respectively.
- Then we initializes a new resultant table with the new metadata such as columns(Here resultant table will have all columns from both the tables).
- Also we update other metadata also like columnCount, maxRowsPerBlock, etc. calculated bases on the new columns.
- We set a cursor to the top rows of each table.
- Based on the condition, we declare a temporary cursor and iterate over the temporary cursor until the condition is true and then move to the next row of the other table and write the output buffer to a new page whenever required(at every step whenever the condition is true).
- Whenever the write buffer is full, we write a new page into the resultant table.
- At the end we insert the new table into the tableCatalogue, and delete both the temporary tables.

## ORDER BY
- First we parse the query and find the sorting attribute and its order.
- Then we initializes a new resultant table with the same metadata & data as the original table.
- Then we call the same external sorting function (implemented during SORT command), with the parsed sorting attribute, and strategy(ASC/DESC).
- At the end we insert the new table into the tableCatalogue.

## GROUP BY
- First we parse the query and find the column at which table needs to be grouped, then aggreagate function(MIN, MAX, SUM, AVG), column, and value on which we need to make comparision, and the return aggreagate function and column that we need to print in the table.
- Then we initializes a new temporary table with the same metadata & data as the original table, and sort the temporary tables based on grouping column.
- Then we initializes a new resultant table with the new metadata such as columns(Here resultant table will have 2 column).
- Also we update other metadata also like columnCount, maxRowsPerBlock, etc. calculated bases on the new columns.
- Current values of the aggregate function, and return function and attribute are tracked according to the aggregation functions (e.g., SUM, MAX, MIN, AVG, etc.).
- Rows are iterated through, and aggregate function values, return function value, count of records of same grouping column (We are using count for AVG function, as for AVG we are taking SUM at runtime) are updated while the grouping attribute remains the same.
- Once the grouping value changes, if the function is AVG, we update the aggregate function or return function value by dividing it by count of rows.
- Then we validate the aggregate function value, and aggregate condition value based on the binary operator. If it turns out to be false, we drop that row, and If it is true, we insert a new row with 2 columns(grouping column value, return function value), and update the statistics. We reset the count, aggregate function, and return function value.
- Whenever the write buffer is full, we write a new page into the resultant table.
- At the end we insert the new table into the tableCatalogue, and delete the temporary table.

# Assumptions
- Any table should not have name started with '$', as the temporary table/blocks are using '$' symbol.
- All data within the tables should be integers without formatting(not comma separated), and not empty.
- It is assumed that all the legacy queries like LOAD, PRINT, CLEAR, etc. are implemented correctly.
- In the Query the integers should not be comma separated, otherwise it will results in SYNTAX FAILURE.
- Aggregate operations should not overflow or underflow over integer ranges.
- All the Query executed should result at lease 1 row.

# Learnings
During this project, we learned several key lessons and best practices, which have greatly enriched our understanding and expertise in database operations and software development:

- **Optimized Data Structures**: Our project has emphasized the critical importance of designing data structures and classes with optimal access and constraints in mind. This design significantly reduces the need for complex computations when accessing specific rows and values within those rows.

- **Syntactic and Semantic Checks**: Robust syntactic and semantic checks were implemented for each query, ensuring that all queries strictly adhere to specified column orders. Additionally, we employed regular expressions when needed to enhance the parsing of queries, ensuring their correctness.

- **Efficient External Sorting**: We enforced strict limitations during the external sorting phase, allowing only one read/write access per block. This approach has been highly effective in optimizing the sorting process, including the handling of sort runs within a block.

- **Exception Handling**: Our project involved the development of mechanisms to gracefully handle exceptional scenarios, such as dealing with empty query results and complex conditions. These mechanisms ensure the robustness of our system under various conditions.

- **Parameter Control**: We exercised control over constructor parameter passing, ensuring that data members are copied when required, rather than relying on pass-by-reference. This approach enhances the modularity and reliability of our code.

- **Complexity Analysis**: We conducted in-depth analysis of time and space complexities, especially concerning logical operators applied to attributes (e.g., greater than, less than, equality). Our findings include the worst-case time complexity for join operations (O(n1 * n2), where n1 and n2 represent the row sizes of the two tables) and the time complexity of external sorting (O(n * log(n)) with a buffer space of 10 blocks). In the context of the "Group by" operation, we identified an additional time complexity of O(n) in the worst case, driven by cursor usage and the maintenance of running aggregation values.

- **Custom Comparator and File Handling**: We honed our skills in creating custom comparators. Additionally, we gained valuable insights into file handling, particularly regarding the renaming of page blocks to exclude outdated reads. These skills are pivotal for ensuring data integrity and system efficiency.

These lessons and best practices have equipped us with a strong foundation for tackling advanced database operations and software development challenges, emphasizing the significance of efficiency, data integrity, and robust error handling. The knowledge gained during this project will undoubtedly prove invaluable for future projects and endeavors.



# Contributions
> [Mayank Gupta](https://github.com/mayankgupta12321) - 33.33% (Semantic and Syntax parsing, SORT Query) \
> [Ardhendu Banerjee](https://github.com/ardhendub) - 33.33% (GROUP BY Query, ORDER BY Query) \
> [Santanu Biswas](https://github.com/SantanuBiswas99) - 33.33% (JOIN Query, Testing)

class Matrix 
{
public:
    string sourceFileName = "";
    string matrixName = "";

    unsigned int matrixSize = 0;
    unsigned int smallMatrixSize = 0;
    unsigned int smallMatrixCount = 0;
    unsigned int blockCount = 0;
    
    
    Matrix();
    Matrix(string matrixName);


    bool load();
    bool blockify();
    void extractMatrixMetadata(string firstline);
    void print();
    void makePermanent();
    bool isPermanent();
    void rename(string newMatrixName);
    vector<vector<int>> transpose(vector<vector<int>>& grid);
    void transposeMatrix();
    bool isSymmetric();
    void unload();
    void getNextPage(Cursor *cursor);

    void writeRow(vector<int> row, ostream &fout, int columnsToWrite, bool isPermanent = false)
    {
        logger.log("Table::printRow");
        for (int columnCounter = 0; columnCounter < columnsToWrite; columnCounter++)
        {
            if (columnCounter != 0){
                if(isPermanent){
                    fout << ",";
                }
                else{
                    fout << " ";
                }
            }                
            fout << row[columnCounter];
        }
        fout << endl;
    }
};
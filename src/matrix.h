#include "matrixBufferManager.h"


class Matrix 
{
public:
    string sourceFileName = "";
    string matrixName = "";

    unsigned int matrixSize = 0;
    unsigned int smallMatrixSize = 0;
    unsigned int smallMatrixCount = 0;
    
    
    Matrix();
    Matrix(string matrixName);


    bool load();
    bool blockify();
    void extractMatrixMetadata(string firstline);
    void print();
    void makePermanent();
    bool isPermanent();
    void rename(string newMatrixName);
    void transpose(vector<vector<int>>& grid);
    void transposeMatrix();
    bool isSymmetric(vector<vector<int>>& grid);
    bool isSymmetric();
    void compute();
    void unload();


    void writeRows(vector<vector<vector<int>>> elementsInPages, int pageRowCounter, int rowsToWrite, int columnsToWrite, ostream &fout)
    {
        logger.log("Matrix::printRows");

        for(int rowCounter = 0; rowCounter < this->smallMatrixSize; rowCounter++) {
            for(int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
                for(int columnCounter = 0; columnCounter < this->smallMatrixSize; columnCounter++) {
                    if(pageRowCounter * this->smallMatrixSize +rowCounter >= rowsToWrite) return;
                    if(pageColumnCounter * this->smallMatrixSize + columnCounter >= columnsToWrite) continue;
                    if(pageColumnCounter != 0 || columnCounter != 0)
                        fout << ",";
                    fout << elementsInPages[pageColumnCounter][rowCounter][columnCounter];
                }
            }
            fout << endl;
        }
    }
};
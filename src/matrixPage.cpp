#include "global.h"

MatrixPage :: MatrixPage()
{
    logger.log("MatrixPage::MatrixPage");
    this->pageName = "";
    this->matrixName = "";
    this->pageRowIndex = -1;
    this->pageColumnIndex = -1;
    this->matrixSize = 0;
    this->smallMatrixSize = 0;
    this->smallMatrixCount = 0;
    this->elementsInPage.clear();
}

MatrixPage::MatrixPage(string matrixName, int pageRowIndex, int pageColumnIndex)
{
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageRowIndex = pageRowIndex;
    this->pageColumnIndex = pageColumnIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Page_" + to_string(pageRowIndex) + "_" + to_string(pageColumnIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->matrixSize = matrix.matrixSize;
    this->smallMatrixSize = matrix.smallMatrixSize;
    this->smallMatrixCount = matrix.smallMatrixCount;

    vector<int> elementsInRow(this->smallMatrixSize, -1);
    this->elementsInPage.assign(this->smallMatrixSize, elementsInRow);

    ifstream fin(pageName, ios::in);
    int number;
    string line, word;
    for (int rowCounter = 0; rowCounter < this->smallMatrixSize; rowCounter++)
    {
        if(!getline(fin, line)) break;
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->smallMatrixSize; columnCounter++)
        {
            if(!getline(s, word, ' ')) break;
            this->elementsInPage[rowCounter][columnCounter] = stoi(word);
        }
    }

    fin.close();
}

MatrixPage::MatrixPage(string matrixName, int pageRowIndex, int pageColumnIndex, vector<vector<int>> elementsInPage, int matrixSize, int smallMatrixSize, int smallMatrixCount)
{
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageRowIndex = pageRowIndex;
    this->pageColumnIndex = pageColumnIndex;
    this->matrixSize = matrixSize;
    this->smallMatrixSize = smallMatrixSize;
    this->smallMatrixCount = smallMatrixCount;
    this->elementsInPage = elementsInPage;
    this->pageName = "../data/temp/" + this->matrixName + "_Page_" + to_string(pageRowIndex) + "_" + to_string(pageColumnIndex);
}

vector<vector<int>> MatrixPage::getPage() {
    logger.log("MatrixPage::writePage");
    return this->elementsInPage;
}

void MatrixPage::writePage()
{
    logger.log("MatrixPage::writePage");
    ofstream fout(this->pageName, ios::trunc);


    int rowsToWrite = this->matrixSize;
    int columnsToWrite = this->matrixSize;


    for (int rowCounter = 0; rowCounter < this->smallMatrixSize; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->smallMatrixSize; columnCounter++)
        {
            if(this->pageRowIndex * this->smallMatrixSize + rowCounter >= rowsToWrite) return;
            if(this->pageColumnIndex * this->smallMatrixSize + columnCounter >= columnsToWrite) continue;
            if (columnCounter != 0)
                fout << " ";
            fout << this->elementsInPage[rowCounter][columnCounter];
        }
        fout << endl;
    }
    
    fout.close();
}




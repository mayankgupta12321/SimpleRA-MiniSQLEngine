class MatrixPage {

    string matrixName;
    int pageRowIndex;
    int pageColumnIndex;
    int matrixSize;
    int smallMatrixSize;
    int smallMatrixCount;
    vector<vector<int>> elementsInPage;

    public:

    string pageName = "";
    MatrixPage();
    MatrixPage(string matrixName, int pageRowIndex, int pageColumnIndex);
    MatrixPage(string matrixName, int pageRowIndex, int pageColumnIndex, vector<vector<int>> elementsInPage, int matrixSize, int smallMatrixSize, int smallMatrixCount);
    vector<vector<int>> getPage();
    void writePage();
};

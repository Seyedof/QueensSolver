#include "QueenBoard.h"

bool operator<(const QueenBoard::Coord& lhs, const QueenBoard::Coord& rhs)
{
    if (lhs.row != rhs.row) {
        return lhs.row < rhs.row;
    }
    else {
        return lhs.column < rhs.column;
    }
}

bool QueenBoard::ValidateBounds(int row, int column)
{
    if (row >= Size() || row < 0) {
        return false;
    }
    if (column >= Size() || column < 0) {
        return false;
    }
    return true;
}

void QueenBoard::Create(int boardSize)
{
    m_boardCV.clear();
    for (int i = 0; i < boardSize; ++i) {
        m_boardCV.push_back(std::vector<int>(boardSize));
    }

    m_boardColor.clear();
    for (int i = 0; i < boardSize; ++i) {
        m_boardColor.push_back(std::vector<uint32_t>(boardSize));
        std::fill(m_boardColor[i].begin(), m_boardColor[i].end(), DEFAULT_BOARD_COLOR);
    }
}

bool QueenBoard::LoadBoard(const std::string& filename)
{
    FILE* file = fopen(filename.c_str(), "rt");
    if (!file) {
        return false;
    }

    int size = 0;
    fscanf(file, "%d\n", &size);

    Create(size);

    for (int row = 0; row < size; row++) {
        for (int column = 0; column < size; column++) {
            uint32_t color;
            fscanf(file, "%ud", &color);
            m_boardColor[row][column] = color;
        }
        fscanf(file, "\n");
    }

    fclose(file);
    return true;
}

bool QueenBoard::SaveBoard(const std::string& filename)
{
    FILE* file = fopen(filename.c_str(), "wt");
    if (!file) {
        return false;
    }

    fprintf(file, "%d\n", static_cast<int>(Size()));

    for (int row = 0; row < Size(); row++) {
        for (int column = 0; column < Size(); column++) {
            fprintf(file, "%ud ", m_boardColor[row][column]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return true;
}

void QueenBoard::PutQueen(int row, int column)
{
    if (!ValidateBounds(row, column)) {
        return;
    }
    
    m_queens.insert({ row, column });
}

void QueenBoard::RemoveQueen(int row, int column)
{
    if (!ValidateBounds(row, column)) {
        return;
    }

    m_queens.erase({ row, column });
}

void QueenBoard::MoveQueen(int row, int column, int newRow, int newColumn)
{
    RemoveQueen(row, column);
    PutQueen(newRow, newColumn);
}

void QueenBoard::SetBoardColor(int row, int column, uint32_t color)
{
    if (!ValidateBounds(row, column)) {
        return;
    }

    m_boardColor[row][column] = color;
}

bool QueenBoard::IsOccupied(int row, int column)
{
    if (!ValidateBounds(row, column)) {
        return false;
    }

    return m_queens.find({ row, column }) != m_queens.end();
}

void QueenBoard::CalcConflictValuesHorz()
{
    for (const auto& queen : m_queens) {
        for (int column = 0; column < Size(); ++column) {
            if (column != queen.column) {
                m_boardCV[queen.row][column]++;
            }
        }
    }
}

void QueenBoard::CalcConflictValuesVert()
{
    for (const auto& queen : m_queens) {
        for (int row = 0; row < Size(); ++row) {
            if (row != queen.row) {
                m_boardCV[row][queen.column]++;
            }
        }
    }
}

void QueenBoard::CalcConflictValuesDiag()
{
    for (const auto& queen : m_queens) {
        ValidateBounds(queen.row - 1, queen.column - 1) ? m_boardCV[queen.row - 1][queen.column - 1]++ : queen.row;
        ValidateBounds(queen.row - 1, queen.column + 1) ? m_boardCV[queen.row - 1][queen.column + 1]++ : queen.row;
        ValidateBounds(queen.row + 1, queen.column - 1) ? m_boardCV[queen.row + 1][queen.column - 1]++ : queen.row;
        ValidateBounds(queen.row + 1, queen.column + 1) ? m_boardCV[queen.row + 1][queen.column + 1]++ : queen.row;
    }
}

void QueenBoard::CalcConflictValuesColor()
{

    int conflictValue = 0;
    for (const auto& queen : m_queens) {
        uint32_t colorQueen = m_boardColor[queen.row][queen.column];
        for (int row = 0; row < Size(); ++row) {
            for (int column = 0; column < Size(); ++column) {
                if (queen.row != row && queen.column != column) {
                    if (m_boardColor[row][column] == colorQueen) {
                        m_boardCV[row][column]++;
                    }
                }
            }
        }
    }
}

void QueenBoard::CalcConflictValues()
{
    CalcConflictValuesHorz();
    CalcConflictValuesVert();
    CalcConflictValuesDiag();
    CalcConflictValuesColor();
}

void QueenBoard::UpdateConflictValues()
{
    for (int row = 0; row < Size(); ++row) {
        std::fill(m_boardCV[row].begin(), m_boardCV[row].end(), 0);
    }

    CalcConflictValues();
}

void QueenBoard::InitialQueensSetup()
{
    ClearQueens();
    for (int column = 0; column < Size(); ++column) {
        PutQueen(column, column);
    }
}

const QueenBoard::Coord QueenBoard::GetQueenInColumn(int column)
{
    if (!ValidateBounds(0, column)) {
        return { 0, 0 };
    }

    for (const auto& queen : m_queens) {
        if (queen.column == column) {
            return { queen.row, column };
        }
    }

    return { 0, 0 };
}

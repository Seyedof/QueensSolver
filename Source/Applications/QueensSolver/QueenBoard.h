#pragma once
#include <vector>
#include <set>
#include <string>

class QueenBoard
{
public:
    const uint32_t DEFAULT_BOARD_COLOR = 0xff606060;

    struct Coord {
        int row;
        int column;
    };

public:
    void Create(int boardSize);
    bool LoadBoard(const std::string& filename);
    bool SaveBoard(const std::string& filename);
    void PutQueen(int row, int column);
    void RemoveQueen(int row, int column);
    void MoveQueen(int row, int column, int newRow, int newColumn);
    const std::vector<std::vector<int>>& GetBoardCV() { return m_boardCV; }
    const std::vector<std::vector<uint32_t>>& GetBoardColor() { return m_boardColor; }
    void SetBoardColor(int row, int column, uint32_t color);
    int Size() { return static_cast<int>(m_boardCV.size()); }
    bool IsOccupied(int row, int column);
    void CalcConflictValues();
    void CalcConflictValuesHorz();
    void CalcConflictValuesVert();
    void CalcConflictValuesDiag();
    void CalcConflictValuesColor();
    bool ValidateBounds(int row, int column);
    void ClearQueens() { m_queens.clear(); }
    void UpdateConflictValues();
    void InitialQueensSetup();
    const std::set<Coord>& GetQueens() { return m_queens; }
    const Coord GetQueenInColumn(int column);

private:
    std::vector<std::vector<int>>       m_boardCV;
    std::vector<std::vector<uint32_t>>  m_boardColor;
    std::set<Coord>                     m_queens;
};

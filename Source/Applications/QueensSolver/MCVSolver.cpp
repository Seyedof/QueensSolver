#include "MCVSolver.h"

void MCVSolver::NextStep(QueenBoard& board)
{
    switch (m_curStep) {
    case CalcCVs:
        board.UpdateConflictValues();
        m_curStep = PickMaxQueenCV;
        break;

    case PickMaxQueenCV:
    {
        auto maxCV = 0;
        for (const auto& queen : board.GetQueens()) {
            int conflictValue = board.GetBoardCV()[queen.row][queen.column];
            if (conflictValue > maxCV) {
                maxCV = conflictValue;
            }
        }

        if (maxCV == 0) {
            m_curStep = Win;
            break;
        }

        std::vector<int> maxCVColumns;
        for (const auto& queen : board.GetQueens()) {
            int conflictValue = board.GetBoardCV()[queen.row][queen.column];
            if (conflictValue == maxCV) {
                maxCVColumns.push_back(queen.column);
            }
        }

        m_pickedQueen = maxCVColumns[rand() % maxCVColumns.size()];
        m_curStep = FindMinEmptyCV;
        break;
    }

    case FindMinEmptyCV:
    {
        auto pickedQueen = board.GetQueenInColumn(m_pickedQueen);
        //int maxCV = 0;
        int minCV = board.Size() * board.Size();
        for (size_t row = 0; row < board.Size(); ++row) {
            if (row != pickedQueen.row) {
                if (board.GetBoardCV()[row][pickedQueen.column] < minCV) {
                    minCV = board.GetBoardCV()[row][pickedQueen.column];
                }
            }
        }

        std::vector<int> minCVRows;
        for (size_t row = 0; row < board.Size(); ++row) {
            if (row != pickedQueen.row) {
                if (board.GetBoardCV()[row][pickedQueen.column] == minCV) {
                    minCVRows.push_back(row);
                }
            }
        }

        m_moveToRow = minCVRows[rand() % minCVRows.size()];
        m_curStep = Move;
        break;
    }

    case Move:
    {
        auto pickedQueen = board.GetQueenInColumn(m_pickedQueen);
        board.MoveQueen(pickedQueen.row, pickedQueen.column, m_moveToRow, pickedQueen.column);
        m_curStep = CalcCVs;
        break;
    }

    case Win:
        break;
    }
}
#pragma once
#include "QueenBoard.h"

class MCVSolver
{
public:
    enum Step {
        CalcCVs,
        PickMaxQueenCV,
        FindMinEmptyCV,
        Move,
        Win,
    };

    void NextStep(QueenBoard& board);
    const Step GetCurStep() { return m_curStep; }
    void Reset() { m_curStep = CalcCVs; }
    const int GetPickedQueen() { return m_pickedQueen; }
    const int GetMoveToRow() { return m_moveToRow; }

private:
    Step m_curStep = CalcCVs;
    int m_pickedQueen;
    int m_moveToRow;
};


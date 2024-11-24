#pragma once
#include "QueenBoard.h"

class MCVSolver
{
public:
    enum Step {
        Start,
        CalcCVs,
        PickMaxQueenCV,
        FindMinEmptyCV,
        Move,
        Win,
    };

    void ProcessStep(QueenBoard& board);
    void NextStep(QueenBoard& board);
    const Step GetCurStep() { return m_curStep; }
    void Reset() { m_curStep = m_nextStep = Start; }
    const int GetPickedQueen() { return m_pickedQueen; }
    const int GetMoveToRow() { return m_moveToRow; }

private:
    Step m_curStep = Start;
    Step m_nextStep = Start;
    int m_pickedQueen;
    int m_moveToRow;
};


#pragma once

template<typename TRollbackFunc>
struct RollbackerRAII {

    TRollbackFunc t;

    RollbackerRAII(TRollbackFunc t) :
        t(t) {}

    ~RollbackerRAII() { t(); }
};

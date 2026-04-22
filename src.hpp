#pragma once
#include <cstddef>
#include <cstring>
#include <cmath>

class BaseJudger {
public:
    BaseJudger(size_t time_limit, size_t memory_limit, const char* answer)
        : answer_(nullptr), time_limit_(time_limit), memory_limit_(memory_limit), score_(0) {
        if (answer) {
            size_t len = std::strlen(answer);
            answer_ = new char[len + 1];
            std::memcpy(answer_, answer, len + 1);
        } else {
            answer_ = new char[1];
            answer_[0] = '\0';
        }
    }

    virtual void Submit(size_t time, size_t memory, const char* output) = 0;

    size_t GetScore() const { return score_; }

    virtual ~BaseJudger() {
        delete[] answer_;
        answer_ = nullptr;
    }

protected:
    char* answer_;
    const size_t time_limit_;
    const size_t memory_limit_;
    size_t score_;

    virtual bool CheckAnswer(const char* output) const {
        return std::strcmp(answer_, output) == 0;
    }
};

class ICPCJudger : public BaseJudger {
public:
    ICPCJudger(size_t time_limit, size_t memory_limit, const char* answer)
        : BaseJudger(time_limit, memory_limit, answer) {}

    void Submit(size_t time, size_t memory, const char* output) override {
        size_t cur = 0;
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) cur = 100;
        if (cur > score_) score_ = cur;
    }
};

class OIJudger : public BaseJudger {
public:
    OIJudger(size_t time_limit, size_t memory_limit, const char* answer)
        : BaseJudger(time_limit, memory_limit, answer), submitted_(false) {}

    void Submit(size_t time, size_t memory, const char* output) override {
        if (submitted_) {
            score_ = 0;
            return;
        }
        submitted_ = true;
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) score_ = 100;
        else score_ = 0;
    }

protected:
    bool CheckAnswer(const char* output) const override {
        // Compare line by line, ignoring trailing spaces (and tabs) at each line end
        const char* a = answer_;
        const char* b = output ? output : "";
        while (true) {
            // find end of current line
            const char* a_end = a;
            const char* b_end = b;
            while (*a_end && *a_end != '\n') ++a_end;
            while (*b_end && *b_end != '\n') ++b_end;

            // trim trailing spaces/tabs
            const char* a_trim = a_end;
            const char* b_trim = b_end;
            while (a_trim > a && (*(a_trim - 1) == ' ')) --a_trim;
            while (b_trim > b && (*(b_trim - 1) == ' ')) --b_trim;

            // lengths
            size_t a_len = static_cast<size_t>(a_trim - a);
            size_t b_len = static_cast<size_t>(b_trim - b);
            if (a_len != b_len) return false;
            if (std::strncmp(a, b, a_len) != 0) return false;

            // if both ended (no more chars)
            if (*a_end == '\0' && *b_end == '\0') return true;
            if ((*a_end == '\0') != (*b_end == '\0')) return false; // different number of lines

            // move to next line (skip newline if present)
            a = (*a_end == '\n') ? (a_end + 1) : a_end; 
            b = (*b_end == '\n') ? (b_end + 1) : b_end;

            // if one string ends but the other has more lines, mismatch handled above; loop continues
            if (*a == '\0' && *b == '\0') return true;
            if ((*a == '\0') != (*b == '\0')) return false;
        }
    }

private:
    bool submitted_;
};

class SpacialJudger : public BaseJudger {
public:
    SpacialJudger(size_t time_limit, size_t memory_limit, size_t full_score_time, size_t full_score_memory, const char* answer)
        : BaseJudger(time_limit, memory_limit, answer), fst_(full_score_time), fsm_(full_score_memory) {}

    void Submit(size_t time, size_t memory, const char* output) override {
        size_t cur = 0;
        if (CheckAnswer(output)) {
            int tscore = calc_score(time, fst_, time_limit_);
            int mscore = calc_score(memory, fsm_, memory_limit_);
            long long prod = static_cast<long long>(tscore) * static_cast<long long>(mscore);
            cur = static_cast<size_t>(prod / 100LL);
        }
        if (cur > score_) score_ = cur;
    }

private:
    size_t fst_;
    size_t fsm_;

    static int calc_score(size_t val, size_t full_score_thres, size_t limit) {
        if (val <= full_score_thres) return 100;
        if (val >= limit) return 0;
        if (full_score_thres >= limit) {
            // Degenerate: treat as step at limit
            return (val <= limit) ? 100 : 0;
        }
        // Linear between thresholds: floor
        size_t numer = (limit - val) * 100;
        size_t denom = (limit - full_score_thres);
        return static_cast<int>(numer / denom);
    }
};

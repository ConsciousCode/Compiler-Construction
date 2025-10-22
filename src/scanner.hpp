#include "token.hpp"

class Scanner {
  private:
    const char* cur;

    void advance(int by = 1);
    void newline();
    void consume_space();
    bool consume_comment();

  public:
    Loc loc;

    Scanner(const char* input);

    Token next_token();
};

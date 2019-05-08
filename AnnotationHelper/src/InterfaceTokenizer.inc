<?php


namespace Inte\Annotation\Helper;



class InterfaceTokenizer extends Tokenizer {
  protected function getInterfaceImplementsKeywordToken(): int {
    return T_EXTENDS;
  }

  protected function getDefineKeywordToken():int {
    return T_INTERFACE;
  }

}
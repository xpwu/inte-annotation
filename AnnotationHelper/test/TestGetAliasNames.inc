<?php


namespace Inte\Test;

require_once "TestGetAliasNamesElseClass.inc";


use Inte\Test\ElseClass\TestGetAliasNamesElseClass as ElseC;

use Inte\Test\ElseClass\TestGetAliasNamesElseClass;


class TestGetAliasNames implements ElseC{
  /**
   * @var TestGetAliasNamesElseClass
   */
  public $a;

  /**
   * @var ElseC
   *   @uses \AnnotationTestClass   ("ok", arg = ['a'; 9; false; TRUE; 9.4])
   */
  public $b;


  /**
   * @var \Inte\Test\ElseClass\TestGetAliasNamesElseClass
   */
  public $c;

  /**
   * @var ElseClass\TestGetAliasNamesElseClass
   */
  public $d;

  /**
   * @var ElseClass\0TestGetAliasNamesElseClass
   */
  public $e;
}

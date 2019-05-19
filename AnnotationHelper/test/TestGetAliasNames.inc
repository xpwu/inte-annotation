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
   *   @uses \AnnotationTestClass   ('o\'k', arg = ['a'; 9; false; TRUE; 9.4])
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

  /**
   * @var integer
   */
  public $f;

  /**
   * @var int|null
   */
  public $g;

  /**
   * @var int[]|double
   */
  public $h;

  /**
   * @var ElseClass\TestGetAliasNamesElseClass[]
   */
  public $i;

  /**
   * @var ElseClass\TestGetAliasNamesElseClass[]|boolean
   */
  public $j;

  /**
   * @var ElseClass\TestGetAliasNamesElseClass[]|boolean[]
   */
  public $k;

  /**
   * @var ElseClass\TestGetAliasNamesElseClass[]|boolean[
   */
  public $l;

  /**
   * @var ElseC
   *   @uses \AnnotationTestClass   (value="ok-value", arg = ['a', 9, false, 5, 9.4])
   */
  public $m;

  /**
   * @var ElseC
   *   @uses \AnnotationTestClass  ('ok')
   */
  public $n;

  /**
   * @var ElseC
   *   @uses \AnnotationTestClass  ('o\'k\'')
   */
  public $o;
}

# inte-annotation
基于php-integrate的注解基础库，此库是对注解使用的进一步细化与支持。

## 注解Helper
提供对类的注解、方法与属性的注解，支持使用PHPDoc的方式使用注解 与 extends/implements的方式使用注解，注解可以使用class或者interface实现，根据不同的场景实现方式与使用方式均有区别。所有的注解都需要被Annotation[元注解](#MetaAnnotation)注解。

### 对类的注解
用class或者interface实现的注解都可以对类做注解，类的注解使用extends/implements的方式，除继承链外遵循php extends/implements 的语法规则。对类的注解中支持Inherited[元注解](#MetaAnnotation)设定继承链。

### 对方法、属性的注解
仅支持PHPDoc的方式使用注解，也仅支持使用class实现的注解。遵循php class 的语法规则，支持使用命名空间，使用interface实现的注解对方法、属性无效。注解可以带参数，使用class的属性的方式实现，也可以不带参数。  
  
如下定义了一个可以使用在方法、属性的注解。

```
<?php
namespace Inte\Demo;
use Inte\MetaAnnotation\Annotaion;

class MethAnnotation extends Annotaion {
  public $value;
  public $name = "xp";
  public $tel;
}
```
注解名叫Inte\Demo\MethAnnotation，有三个属性，其中name有值为'xp'的默认值。
注解中属性的类型只能是 bool,string,int,float,null,[string,int,bool,float,null] 五种类型，支持的参数满足如下的正则表达式  
```
'/'
. '(\s*'
. '([a-zA-Z_][a-zA-Z0-9_]*)'
. '\s*=)?\s*?'
. '((\'(([^\'])|(\\\\\'))*[^\\\\]\')'
. '|("(([^"])|(\\\\"))[^\\\\]*")'
. '|(\[[^\]]*\])'
. '|([^\'"\[,;=\s]+))'
. '\s*?,?'
. '/';

```
数组支持如下的正则表达式  
```
'/'
. '((\'(([^\'])|(\\\\\'))*[^\\\\]\')'
. '|("(([^"])|(\\\\"))[^\\\\]*")'
. '|([^\'",;=\s]+))'
. '\s*?(,|;)?'
. '/';
```

  
注解的使用需要在对应属性或者方法的注释块中， **\* @uses** 标记开始，然后写注解名，注解名与注解标记之间用一个或者多个 **空格/tab** 分隔。如果需要参数，后面跟上参数的赋值，参数与注解名之间用一个或者多个 **空格/tab** 分割，参数用一对 **()** 表示，多个参数之间用 **,** 分隔，一个参数使用 **argName=argValue** 的方式表示。  
一个注解独占一行，一个属性或者方法可以有多个注解。
举例如下：  
   
```

use Inte\Demo\MethAnnotation;

class Test {
  /**
  *
  * @uses MethAnnotation (value=5, name="xpwu", tel=['1890000'; '145999']) 
  */
  private $myproperty;
}
```
  
在注解处理器中如下使用   
   
```
...
$propertyRef = ... 
$helper = new Helper(new ReflectionClass(MethAnnotation::class), $logger);
/**
*@var MethAnnotation $annotaion
*/
$annotaion = $helper->getAnnotationAssociatedWithProperty($propertyRef);
$logger->info($annotaion->value);
$logger->info($annotaion->name);
...

```
  
以上会输出  5, xpwu   
  
如果注解的属性名为value，则在使用注解时，可以不写参数名，对上例也可这样使用，有完全相同的效果。

```

use Inte\Demo\MethAnnotation;

class Test {
  /**
  *
  * @uses MethAnnotation (5, name="xpwu", tel=['1890000'; '145999']) 
  */
  private $myproperty;
}
```
  
如果注解属性有默认值，在使用注解时，也可以不对参数赋值，则会使用默认值。  
```

use Inte\Demo\MethAnnotation;

class Test {
  /**
  *
  * @uses MethAnnotation (5, tel=['1890000'; '145999']) 
  */
  private $myproperty;
}
```

```
...
$propertyRef = ... 
$helper = new Helper(new ReflectionClass(MethAnnotation::class), $logger);
/**
*@var MethAnnotation $annotaion
*/
$annotaion = $helper->getAnnotationAssociatedWithProperty($propertyRef);
$logger->info($annotaion->value);
$logger->info($annotaion->name);
...

```
  
以上会输出  5, xp  
  

## <a name="MetaAnnotation"></a>元注解
元注解是对注解的注解

### Annotation
所有的注解都需要Annotation进行注解，用implements或者extends的方式实现。

### Inherited
被Inherited注解的注解表示此注解是否可被继承，主要是用在对类的注解上。默认对类的注解不具有继承性。父类或者接口使用了某一个注解，子类或者实现类默认是没有使用此注解的，除非显示的使用了注解。如果一个注解使用了Inherited元注解，表示子类或者实现类也自动使用了此注解。
Inherited注解本身不具有继承性，也就是或一个注解A继承(或扩展)自另一个注解C，而C被Inherited注解，但是A并不具有Inherited属性，除非显示指定Inherited属性。


## 发布检查
检查发布的lib名与代码的类名是否匹配。





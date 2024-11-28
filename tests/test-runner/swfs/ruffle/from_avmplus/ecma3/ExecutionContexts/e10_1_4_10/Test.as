/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package {
import flash.display.MovieClip; public class Test extends MovieClip {}
}

import com.adobe.test.Assert;
//     var SECTION = "10.1.4-10";
//     var VERSION = "ECMA_1";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;


    var VALUE = 12345;
    var MYOBJECT = new Number( VALUE );

    array[item++] = Assert.expectEq( "MYOBJECT.toString()",  String(VALUE), MYOBJECT.toString()); 


    return ( array );
}

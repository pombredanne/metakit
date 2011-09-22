// tstore5.cpp -- Regression test program, storage tests, part 5
// $Id: tstore5.cpp 1266 2007-03-09 16:52:46Z jcw $
// This is part of MetaKit, see http://www.equi4.com/metakit/

#include "regress.h"

void TestStores5()
{
  B(s40, LoadFrom after commit, 0) W(s40a);
  {
    c4_IntProp p1 ("p1");

    { // create datafile by streaming out
      c4_Storage s1;
      s1.SetStructure("a[p1:I]");

      c4_View v1 = s1.View("a");
      v1.Add(p1 [123]);
	A(p1 (v1[0]) == 123);
	A(v1.GetSize() == 1);

      c4_FileStream fs1 (fopen("s40a", "wb"), true);
      s1.SaveTo(fs1);
    }
    { // it should load just fine
      c4_Storage s2;
      c4_FileStream fs1 (fopen("s40a", "rb"), true);
      bool ok = s2.LoadFrom(fs1);
      	A(ok);

      c4_View v1 = s2.View("a");
	A(p1 (v1[0]) == 123);
	A(v1.GetSize() == 1);
    }
    { // open the datafile and commit a change
      c4_Storage s3 ("s40a", true);

      c4_View v1 = s3.View("a");
	A(p1 (v1[0]) == 123);
	A(v1.GetSize() == 1);
      p1 (v1[0]) = 456;
      s3.Commit();
	A(p1 (v1[0]) == 456);
	A(v1.GetSize() == 1);
    }
    { // it should load fine and show the last changes
      c4_Storage s4;
      c4_FileStream fs1 (fopen("s40a", "rb"), true);
      bool ok = s4.LoadFrom(fs1);
      	A(ok);

      c4_View v1 = s4.View("a");
	A(p1 (v1[0]) == 456);
	A(v1.GetSize() == 1);
    }
    { // it should open just fine in the normal way as well
      c4_Storage s5 ("s40a", false);
      c4_View v1 = s5.View("a");
	A(p1 (v1[0]) == 456);
	A(v1.GetSize() == 1);
    }
  } D(s40a); R(s40a); E;

    // 2002-03-13: failure on Win32, Modify calls base class GetNthMemoCol
  B(s41, Partial modify blocked, 0) W(s41a);
  {
    c4_BytesProp p1 ("p1");
    c4_Storage s1 ("s41a", true);
    c4_View v1 = s1.GetAs("a[_B[p1:B]]");

    // custom viewers did not support partial access in 2.4.3
    c4_View v2 = v1.Blocked();
    s1.Commit();

    v2.SetSize(1);

    c4_BytesRef m = p1 (v2[0]);
    m.Modify(c4_Bytes ("abcdefgh", 8), 0);

    s1.Commit();

  } D(s41a); R(s41a); E;

  B(s42, Get descriptions, 0)
  {
    c4_Storage s1;
    s1.SetStructure("a[p1:I],b[p2:S]");
    
      c4_String x1 = s1.Description();
      A(x1 == "a[p1:I],b[p2:S]");
    
      c4_String x2 = s1.Description("b");
      A(x2 == "p2:S");
    
      const char* cp = s1.Description("c");
      A(cp == 0);
  } E;

    // 2002-04-24: VPI subview ints clobbered
  B(s43, View reuse after sub-byte ints, 0) W(s43a);
  {
    c4_IntProp p1 ("p1");
    c4_Storage s1 ("s43a", true);
    c4_View v1 = s1.GetAs("a[p1:I]");

    v1.Add(p1 [0]);
    v1.Add(p1 [1]);
    s1.Commit();

    v1.SetSize(1); // 1 is an even trickier bug than 0
    s1.Commit();

    // adding the following two lines works around the 2.4.4 bug
    //s1.Rollback();
    //v1 = s1.GetAs("a[p1:I]");
    
    v1.Add(p1 [12345]);
    s1.Commit();
    
      int n = p1 (v1[1]);
      A(p1 (v1[1]) == 12345);

  } D(s43a); R(s43a); E;
}

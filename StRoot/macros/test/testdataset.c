{
 gROOT->Reset();
  // Determinate the brand of the OS
  gSystem.Load("St_base");

  St_DataSetIter d;
  d.Mkdir("v1/v1_1/v1_1_1");
  cout << "Short list:" << endl;
  d.Pwd();  
  cout << "Wide list:" << endl;
  d.Pwd("*");
                                  cout << endl;
  d.Mkdir("/v1/v1_2/v1_2_1");
  d.Pwd("*");
                                  cout << endl;
  d.Mkdir("v1/v1_3/v1_2_2");

  St_DataSet *last = d.Mkdir("v1/v1/v1_v1_2_2");
  St_DataSet *just = d("v1/v1/v1_v1_2_2");

  if (last != just) 
        cout << "Error: Mkdir return a wrong addrees";
   else    
        cout << "Ok! return value of St_DataSetIter::Mkdir method has been tested";
        
   cout << endl << endl;     

  d.Pwd("*");
                                  cout << endl;
  d.Mkdir("v1/v21/v3211");
  d.Pwd("*");
  cout  << endl << "------------ 1 ------------ " << endl;
                                  cout << endl;
  d.Mkdir("v1/v21/v3212/v4");
  cout << "the current path: " << d("v1/v21/v3212/v4")->Path() << endl;
  d.Pwd("/");
  d("/")->ls();
  d.ls("/");
  cout << "list \"v3212\" the relative path " << endl;
  d.Pwd("v1/v21/v3212");
  d("v1/v21/v3212")->ls();
  d.ls("v1/v21/v3212");
                                  cout << endl;
  d.Du();
                                  cout << endl;
  cout << "We'll try some \"wrong\" path now" << endl;
  d.Ls("unknown");
//  d.Pwd()->ls("unknown");
//  cout  << endl << "------------ 2 ------------ " << endl;
  cout  << endl << "-------- 2 test \"FindObject\" ------------ " << endl;

  const Char_t *v3212 = "v3212";
  St_DataSet *found = d.FindObject(v3212);
  if (found) {
      const Char_t *t = found->Path();
      cout << "Object: " << v3212 << " found. It\'s path is " << t << endl;
  }
  else 
      cout <<  "Object: " << v3212 << " has not been found." << endl;

  cout  << endl << "-------- 2.2 test \"FindObject\" ------------ " << endl;
 
  const Char_t *vS3213 = "/v32/13";
  cout << " ===   Error message expected " << endl;
  found = d.FindObject(vS3213);

  cout  << endl << "-------- 2.3 test \"FindObject\" ------------ " << endl;

  const Char_t *V3212 = "V3212";
  found = d.FindObject(V3212,0,"-I");
  if (found) {
       const Char_t *t2 = found->Path();
       cout << "Object: " << V3212 << " found. It\'s path is " << t2 << endl;
  }
  else
       cout <<  "Object: " << V3212 << " has not been found." << endl;

  cout  << endl << "-------- 2.4 test \"FindObject\" ------------ " << endl;

  cout << " ===   Error message expected " << endl;
  found = d.FindObject(V3212,"v1/v21/v3212/v4","-I");
  if (!found) cout << " Object not found" << endl;

  cout << "recreating directories" << endl;

  d.Rmdir("v1/v1_1/v1_1_1");
  d.Pwd()->ls("*");
  cout  << endl << "------------ 3 ------------ " << endl;
  d.Mkdir("v1/v1_1/v1_1_1");
  d.Pwd()->ls("*");
  cout  << endl << "------------ 4 ------------ " << endl;
  d.Rmdir("v1/v1_1/v1_1_1");
  d.Pwd()->ls("*");
  cout  << endl << "------------ 5 ------------ " << endl;
  d.Mkdir("v1/v1_1/v1_1_1");
  d.ls("/v1",3);
  cout  << endl << "------------ 6 ------------ " << endl;
  d.Rmdir("v1/v21");
  cout  << endl << "------------ 7 ------------ " << endl;
  d.ls("","*");
  d.Rmdir("v1");
  d.Rmdir("v1");
  if (d.Cwd()) { 
     d.ls("","*");
     cout << "Error the directory should be undefined nowadays " << endl;
  }
  else cout << "Ok! The last dataset has NO active directory anymore" << endl;

  cout  << endl << "------------ 8 ------------ " << endl;
  
}

# TextMap
TexMap is a data interface easy to write and easy to use in the code to access to any data.
The mecanisim is very easy to understand there is a key and data, but the key is data as well.

To create the TextMap, you only need to create a instance with the text string of the data.
 
	String data="Members{  John  {    DOB{ 11 November 1991 }  }  Peter  {    DOB{ 1 July 1961 }  }}";
	TextMap tmMesh(data);

To access to any data of the hierarchy you can do it with three differnt methods
* method 1  
	TextMap members=data.Get("Members");
* method 2  
	TextMap members=data["Members"];
* method 3  
	TextMap members=data / "Members";
 
For example if you want to read the DOB of peter you can do this  
  TextMap peter_dob=data / "Members" / "Peter" / "DOB"
  
Then you can access to each elemt of peter_dob by index with three methods as well
* method 1  
  TextMap day=peter_dob.Get(0);
* method 2  
  TextMap month=peter_dob[1];
* method 3  
  TextMap year=peter_dob / 2;  
  
 * Finally you can convert this data to: string, int, float, bool  
  int day=peter_dob / 0;  
  string month=peter_dob / 1;  
  float year=peter_dob / 2;  
    
* Or you can do this   
  int day=(peter_dob / 0).As(0);  
  string month=(peter_dob / 1).As("");  
  float year=(peter_dob / 2).As(0.0f);  
  
* Or you can do this   
  int day=(int)(peter_dob / 0);  
  string month=(String)(peter_dob / 1);  
  float year=(float)(peter_dob / 2);  
  
  
* If you want to get how many members there are, you can do this  
  int n = data["Members"].Count();  
  
* And then you can access by index   
  for(int i=0;i<n;i++)  
  {  
    TextMap member=data / "Members" / i;  
    cout << "Member index " << i << " is " (String)member " DOB " << (int)(member/0) << " " << (String)member/1 << " " (int)(member/2);    }  
  

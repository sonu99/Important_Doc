Note:- For any modification in file you need to bring 1st file into stream and do string manupulation.
       Steps:
       1> Calculate the size of file.
                  fseek(fp,0,2);
                  size=ftell(fp)+1;
                  fseek(fp,0,0);
       2> Allocate same size memory 
                  str=calloc(1,size);
       3> read file data and put it in to allocated memory.
       4> Now you can do string manupulation.
       
:::::::::::::::::::::::::::::::::::::::::::::::::::Preprocessor::::::::::::::::::::::::::::::::::::::::

                  

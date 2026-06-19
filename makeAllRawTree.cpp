#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <filesystem>

#include "AndCommon.h"
std::vector<std::string> get_filenames( std::filesystem::path path );

int main(int argc, char *argv[]){

  //parametri da input
  if (argc!=2){
    std::cout << "USAGE: ./makeAllrawTree [folder]" << std::endl;
     std::cout << "EXAMPLE: ./makeAllrawTree data_test" << std::endl;
    exit(1);
  }
  char* meas = argv[1] ;

  std::stringstream stream;    
  
  std::string path = "data/trc/" + std::string(meas); 
  std::vector<std::string> filenames = get_filenames(path);
  
  for(int i=0; i<filenames.size(); i++){
  
    stream << "./makeRawTree " << meas << " " << filenames[i] <<  std::endl;
    
    //senza if riesegue per ogni ciclo anche tutti i programmi prima dell'i-esimo
    if(i==filenames.size()-1){
      system(stream.str().c_str());
    }
  }
  
  return 0;
}


 
std::vector<std::string> get_filenames( std::filesystem::path path ){

  namespace stdfs = std::filesystem;  
  std::vector<std::string> filenames;
  const stdfs::directory_iterator end{} ;
  
  for( stdfs::directory_iterator iter{path} ; iter != end ; ++iter ){ 
      filenames.push_back( iter->path().string() ) ;
    }
  
  return filenames;
}

  
 
        

       

       

      

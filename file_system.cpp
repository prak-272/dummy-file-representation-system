/**
 * @file main.cpp
 * @author prakhar kumar
 * @date 14 July 2024
 * @brief A virtual implementation of File Management System
 */

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
#include <vector>
#include <atomic>
#include <memory>

class File;
class Folder;
class FileStorage;
class FileManager;

/**
 * @brief 
 */
class File{
private: 
   
   friend class FileManager;
   friend class Folder;

   struct Metadata{
      std::size_t fileSize;
      std::string filePath;
      std::string fileExtension;
      const std::string fileId;
      std::string fileName;
      std::string fileVersion;

      Metadata(std::size_t fileSize,const std::string& filePath,const std::string& fileExtension,const std::string& fileId,const std::string& fileName,const std::string& fileVersion)
       : fileSize{fileSize},filePath{filePath},fileExtension{fileExtension},fileId{fileId},fileName{fileName},fileVersion{fileVersion}{}

   };

   Metadata metadata;
   std::string fileContent;

   File(const std::string& fileId,const std::string& filePath,const std::string& fileExtension,const std::string& fileContent,const std::string& fileName,const std::string& fileVersion)
    : metadata{fileContent.size(),filePath,fileExtension,fileId,fileName,fileVersion},fileContent{fileContent} {}

   void updateContent(const std::string& newFileContent){
     fileContent = newFileContent;
     metadata.fileSize = newFileContent.size();
   }

   void updatePath(const std::string& newFilePath){
      metadata.filePath = newFilePath;
   } 

   void updateName(const std::string& newFileName){
      metadata.fileName = newFileName;
   }

   void printMetadata() const{
      std::cout << "Metadata" << std::endl;
      std::cout << "File Size : " << metadata.fileSize << std::endl;
      std::cout << "File Path : " << metadata.filePath << std::endl;
      std::cout << "File Extension : " << metadata.fileExtension << std::endl;
      std::cout << "File Id : " << metadata.fileId << std::endl;
      std::cout << "File Name : " << metadata.fileName << std::endl; 
      std::cout << "File Version : " << metadata.fileVersion << std::endl;
   }

   void printContent() const{
      std::cout << "File Content : " << std::endl << fileContent << std::endl;
   }
   
};


class Folder{
private:
   
   friend class FileStorage;
   friend class FileManager;
   
   struct Metadata{
     int uniqueFolderCount;
     int uniqueFileCount;
     std::string folderPath;
     std::string folderId;
     std:: string folderName;

     Metadata(int uniqueFolderCount,int uniqueFileCount,const std::string& folderPath,const std::string& folderId,const std::string& folderName)
      : uniqueFolderCount{uniqueFolderCount},uniqueFileCount{uniqueFileCount},folderPath{folderPath},folderId{folderId},folderName{folderName}{}

   };
   
   Metadata metadata;
   Folder* parentFolder;
   std::map<std::string,Folder*> foldersById;
   std::multimap<std::string,Folder*> foldersByName;
   std::map<std::string,File*> filesById;
   std::multimap<std::string,File*> filesByName;
   
   Folder(const std::string& folderId,const std::string& folderPath,Folder* parentFolder,const std::string& folderName)
    : metadata{0,0,folderPath,folderId,folderName}, parentFolder{parentFolder} {
        if(parentFolder != nullptr){
         foldersById["-1"] = parentFolder;                                     // using -1 to get to parent folder
         foldersByName.insert(std::make_pair("..",parentFolder));           // using .. to get to parent folder
        }
    }

   void addFolder(Folder* newFolderPointer){

      const std::string newFolderName = newFolderPointer->metadata.folderName; 

      foldersById[newFolderPointer->metadata.folderId] = newFolderPointer;
      foldersByName.insert(std::make_pair(newFolderName,newFolderPointer));
      
      if(foldersByName.count(newFolderName) == 1)
        metadata.uniqueFolderCount++; 

   }

   void addFile(File* newFilePointer){
      const std::string newFileName = newFilePointer->metadata.fileName;  

      std::cout << newFilePointer->metadata.fileId;

      filesById[newFilePointer->metadata.fileId] = newFilePointer;
      filesByName.insert(std::make_pair(newFilePointer->metadata.fileName,newFilePointer));

      if(filesByName.count(newFileName) == 1)
         metadata.uniqueFileCount++;

   }

   void removeFolder(const std::string& folderName){
      auto folders = foldersByName.equal_range(folderName);

      for(auto folder=folders.first;folder!=folders.second;folder++){
         std::string folderId = folder->second->metadata.folderId;
         foldersById.erase(folderId);

         delete folder->second;
      }
      
      foldersByName.erase(folderName);
      metadata.uniqueFolderCount--; 
      
   }

   void removeFile(const std::string& fileName){
      auto files = filesByName.equal_range(fileName);

      for(auto file=files.first;file!=files.second;file++){
         std::string fileId = file->second->metadata.fileId;
         filesById.erase(fileId);
         
         delete file->second;
      }
      
      filesByName.erase(fileName);
      metadata.uniqueFileCount--; 

   } 

   void printMetadata() const{
      std::cout << "Metadata" << std::endl;
      std::cout << "Folder Path : " << metadata.folderPath << std::endl;
      std::cout << "Folder Count : " << metadata.uniqueFolderCount << std::endl;
      std::cout << "File Count : " << metadata.uniqueFileCount << std::endl;
      std::cout << "Folder Name : " << metadata.folderName << std::endl;
   }

   void printContent() const{
       std::cout << "Folders : " << std::endl;

       for(const auto& folder:foldersById)
           std::cout << folder.first << " ( " << folder.second->metadata.folderName << " ), " << "  ";
       std::cout << std::endl;    
       
       std::cout << "File : " << std::endl;

       for(const auto& file:filesById)
           std::cout << file.first << " ( " << file.second->metadata.fileName << " ), " << "  ";
       std::cout << std::endl; 

   }

public:

   ~Folder() noexcept{

      for(const auto& folder:foldersById){
         if(folder.first == "-1")
          continue;

         delete folder.second;  
      } 

      for(const auto& file:filesById){
         delete file.second;
      }

   }   

};

class FileStorage{

private:
   Folder* rootFolder;

public:

   FileStorage(){
      rootFolder = new Folder("0","/",nullptr,"root");
   }
   
   Folder* getRootFolder() const{
     return rootFolder;
   }

   ~FileStorage() noexcept{
      delete rootFolder;
      std::cout << "File Management System is deleted" << std::endl;  
   }  

};


class FileManager{
    
   FileStorage* fileStorage;
   Folder* currentDirectoryPointer;
   std::string currentDirectoryPath;


   static std::string getFileExtension(const std::string& fileName){
       
      std::size_t fileNameSize = fileName.size();

      for(std::size_t i=fileNameSize-1;i>=0;i--){
         if(fileName[i] == '.'){
            
            std::string fileExtension;

            for(int j=i+1;j<fileNameSize;j++){
               fileExtension += fileName[j]; 
            }

            return fileExtension;

         }
      }

      return "";
   }
     
 
   static void throwErrorifNameInvalid(const std::string& name){
       
      if(name.find('/') != std::string::npos){
         throw std::runtime_error("Folder or File Name cannot contain \"/\" in them");
      }
      else if( !((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z')) ){
         throw std::runtime_error("File or Folder name must start with an alphabet");
      }

   }
   
 
   static std::string getNewFolderOrFileVersion(const std::string& previousFolderOrFileVersion){

      std::string newFolderOrFileVersion = previousFolderOrFileVersion;

      if(newFolderOrFileVersion[1] == '9'){
         newFolderOrFileVersion[0] += 1;
         newFolderOrFileVersion[1] = '0';
      }
      else{
         newFolderOrFileVersion[1] += 1;
      }

      return newFolderOrFileVersion;

   }

   std::string getNewFolderId(const std::string& newFolderName){

      std::string parentFolderId =  currentDirectoryPointer->metadata.folderId;
      std::string newFolderId;

      auto folders = currentDirectoryPointer->foldersByName.equal_range(newFolderName);

      if(folders.first == folders.second){

         std::string folderUniqueCount = std::to_string((currentDirectoryPointer->metadata.uniqueFolderCount+1));

         if(stoi(folderUniqueCount) >= 100)
            throw std::runtime_error("Folder can not be created");

         if(folderUniqueCount.size() == 1)
            newFolderId = parentFolderId + "0" + folderUniqueCount + "00";
         else
            newFolderId = parentFolderId + folderUniqueCount + "00";     
         }  
         else if(folders.first != folders.second){  

           auto lastFolder = --folders.second;
           std::size_t lastFolderIdSize = lastFolder->second->metadata.folderId.size(); 
           
           std::string newFolderUniqueCount = lastFolder->second->metadata.folderId.substr(lastFolderIdSize-4,2);

           std::string previousFolderVersion = lastFolder->second->metadata.folderId.substr(lastFolderIdSize-2,2);
           std::string newFolderVersion = getNewFolderOrFileVersion(previousFolderVersion);

           newFolderId = parentFolderId  +  newFolderUniqueCount +  newFolderVersion; 

         }   
         
         return newFolderId;
   }

   std::string getNewFileId(const std::string& newFileName){
       
      std::string parentFolderId =  currentDirectoryPointer->metadata.folderId;
      std::string newFileId;

      auto files = currentDirectoryPointer->filesByName.equal_range(newFileName);

      if(files.first == files.second){

         std::string fileUniqueCount = std::to_string((currentDirectoryPointer->metadata.uniqueFileCount+1));
        
         if(stoi(fileUniqueCount) >= 100)
            throw std::runtime_error("Folder can not be created");

         if(fileUniqueCount.size() == 1)
            newFileId = parentFolderId + "0" + fileUniqueCount + "00";
         else
            newFileId = parentFolderId + fileUniqueCount + "00";     
         }  
         else if(files.first != files.second){  

           auto lastFile = --files.second;
           std::size_t lastFileIdSize = lastFile->second->metadata.fileId.size(); 
           
           std::string newFileUniqueCount = lastFile->second->metadata.fileId.substr(lastFileIdSize-4,2);

           std::string previousFileVersion = lastFile->second->metadata.fileId.substr(lastFileIdSize-2,2);
           std::string newFileVersion = getNewFolderOrFileVersion(previousFileVersion);

           newFileId = parentFolderId  +  newFileUniqueCount +  newFileVersion; 

         }   
   
         return newFileId;

   }

public:

   FileManager(FileStorage* fileStorage):fileStorage(fileStorage),currentDirectoryPointer{fileStorage->getRootFolder()},currentDirectoryPath{"/"}{}


   void changeDirectory(const std::string& destinationFolder,int version,bool relative){

      if(destinationFolder == ".."){
         currentDirectoryPointer = currentDirectoryPointer->parentFolder;
         currentDirectoryPath = currentDirectoryPointer->metadata.folderPath;

         return;
      }

      Folder* temporaryDirectoryPointer = currentDirectoryPointer; 
      std::vector<std::string> destinationFolderComponents;

      if(relative == false){
         temporaryDirectoryPointer = fileStorage->getRootFolder();
      } 

      try{
         
         if(temporaryDirectoryPointer->foldersByName.count(destinationFolder) == 0)
           throw std::runtime_error("Destination Folder cannot be found");
         
      
         auto folders = temporaryDirectoryPointer->foldersByName.equal_range(destinationFolder);

         int folderIdSize =  folders.first->second->metadata.folderId.size(); 
         std::string folderId = folders.first->second->metadata.folderId.substr(0,folderIdSize-2);

         std::string folderVersion = std::to_string(version-1);
         if(folderVersion.size() == 1)
          folderVersion = '0' + folderVersion;

         folderId = folderId + folderVersion; 

         temporaryDirectoryPointer = temporaryDirectoryPointer->foldersById[folderId]; 
         
         currentDirectoryPointer = temporaryDirectoryPointer;
         currentDirectoryPath = currentDirectoryPointer->metadata.folderPath;

      }
      catch(const std::runtime_error &e){
         std::cout << "Unable to change Direectory " << e.what() << std::endl;
      }

   }
   
  
   void printCurrentDirectory(){

      std::cout << "Current Directory " << currentDirectoryPath<< std::endl;

   }
   
 
   void createFolder(const std::string& newFolderName){

      try{

         throwErrorifNameInvalid(newFolderName);

         std::string newFolderPath = currentDirectoryPath;
         if(currentDirectoryPath == "/"){
            newFolderPath += newFolderName;
         }
         else{
            newFolderPath += "/" + newFolderName;
         }
         
         std::string newFolderId = getNewFolderId(newFolderName);

         Folder* newFolderPointer = new Folder(newFolderId,newFolderPath,currentDirectoryPointer,newFolderName);
         currentDirectoryPointer->addFolder(newFolderPointer);

      } 
      catch(const std::runtime_error &e){
         std::cerr << "Error in creating Folder" << e.what() << std::endl;
      }

   }
   

   void createFile(const std::string& newFileName,const std::string& newFileContent){

     try{
        
      throwErrorifNameInvalid(newFileName);

      std::string newFilePath = currentDirectoryPath;
      if(newFilePath == "/"){
         newFilePath += newFileName;
      }
      else{
         newFilePath += "/" + newFileName;
      }

      std::string newFileId = getNewFileId(newFileName);

      std::string newFileExtension = getFileExtension(newFileName); 

      int newFileIdSize = newFileId.size();  
      std::string newFileVersion = newFileId.substr(newFileIdSize-2,2);

      File* newFilePointer = new File(newFileId,newFilePath,newFileExtension,newFileContent,newFileName,newFileVersion);
      currentDirectoryPointer->addFile(newFilePointer);

     } 
     catch(const std::runtime_error &e){
      std::cout << "Error in Creating File " << newFileName << ": " << e.what() << std::endl;
     }

   }
   
 
   void deleteFolder(const std::string& folderName){
       
      try{

         throwErrorifNameInvalid(folderName);
        
         if(currentDirectoryPointer->foldersByName.count(folderName) == 0 )
          throw std::runtime_error("Folder with specified Name does not exist");
        
         currentDirectoryPointer->removeFolder(folderName);

      }
      catch(const std::runtime_error &e){
         std::cerr << "Erorr in deleting Folder : " << e.what() << std::endl;
      }

   }
  
   void deleteFile(const std::string& fileName){

      try{

         throwErrorifNameInvalid(fileName);
         
         if(currentDirectoryPointer->filesByName.count(fileName) == 0)
          throw std::runtime_error("File with specified name does not exist in memeory");

         currentDirectoryPointer->removeFile(fileName);

      }
      catch(const std::runtime_error &e){

         std::cout << "Error in deletingFile : " << e.what() << std::endl;

      }

   }

   void printFolderMetadata() const noexcept{

      currentDirectoryPointer->printMetadata();

   }

   void printFolderContent() const noexcept{

      currentDirectoryPointer->printContent();

   }  
};

int main()
{
   FileStorage* fileStorage = new FileStorage();
   FileManager* fileManager = new FileManager(fileStorage);

   fileManager->printCurrentDirectory();
   fileManager->createFolder("aaa");
   fileManager->createFile("aaa","bbb");
   fileManager->createFolder("bbb");
   fileManager->createFile("aaa","fffffffff");
   fileManager->changeDirectory("aaa",1,true);
   fileManager->printCurrentDirectory();
   fileManager->changeDirectory("..",1,true);
   fileManager->printCurrentDirectory(); 
   fileManager->printFolderMetadata();
   fileManager->changeDirectory("aaa",1,true);
   fileManager->printFolderMetadata();
   fileManager->changeDirectory("..",1,true);
   fileManager->createFolder("aaa"); 
   fileManager->printFolderMetadata(); 
   fileManager->deleteFile("aaa");
   fileManager->printFolderMetadata();
   fileManager->changeDirectory("aaa",2,true);
   fileManager->createFile("aaa","bbb");
   fileManager->createFolder("bbb");
   fileManager->createFile("aaa","fffffffff");
   fileManager->changeDirectory("..",1,true);
   fileManager->changeDirectory("aaa",1,true);
   fileManager->printFolderMetadata();
   fileManager->changeDirectory("..",1,true);
   fileManager->changeDirectory("aaa",2,true);
   fileManager->printFolderMetadata();


}
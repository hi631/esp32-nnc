#include "FS.h"
#include "SPIFFS.h"

extern "C" {
  #include "src\nnc_main.h"
}

void setup() {            
  Serial.begin(115200); Serial.println(""); 
  Serial.print("Start\n");    
  if (!SPIFFS.begin(true)) { Serial.println("SPIFFS Mount Failed"); return; } 
  listDir(SPIFFS, "/", 0); printf("\n");

  nnc_main();
  
  Serial.print("stop\n");
  while(1); // Stop     
}

// the loop routine runs over and over again forever:
void loop() {
}

File spf;
int checkFile(fs::FS &fs, const char * path){
  spf = fs.open(path);
  if(!spf || spf.isDirectory()) return 0; else return 1;
}
extern "C" int os_fopen(char fn[], char md[]) { int rc = checkFile(SPIFFS, fn); return rc; }
extern "C" int os_fseek(File *fp, int slen, int smd) { return spf.seek( slen, SeekSet); }
extern "C" int os_fread(unsigned char buf[], int siz, int len, File *fp) { 
  int rc = spf.read( buf, siz*len);  return rc/siz; }
extern "C" int os_fgets(unsigned char buf[], File *fp) { 
  String rd = spf.readStringUntil('\n');
  rd.toCharArray((char *)buf, rd.length());  return rd.length();}
extern "C" int os_fwrite(unsigned char buf[], int siz, int len, File *fp) { return spf.write( buf, siz*len); }
extern "C" void os_fclose(File *fp) {spf.close();}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  printf("Spiffs.Dir.Info\n");
  File root = fs.open(dirname);
  File file = root.openNextFile();
    while(file){
      Serial.printf(" %s %d byte\n", file.name(), file.size());
      file = root.openNextFile();
    }
}

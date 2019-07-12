#include "FS.h"
#include "SPIFFS.h"
#include "esp_camera.h"

extern "C" int nnc_main(unsigned char* bdp);

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  printf("Spiffs.Dir.Info\n");
  File root = fs.open(dirname);
  File file = root.openNextFile();
    while(file){
      Serial.printf(" %s %d byte\n", file.name(), file.size());
      file = root.openNextFile();
    }
}

void nnc_setup() {            
  Serial.begin(115200); Serial.println(""); 
  Serial.print("Setup_NNC\n");    
  if (!SPIFFS.begin(true)) { Serial.println("SPIFFS Mount Failed"); return; } 
  //listDir(SPIFFS, "/", 0); printf("\n");
}

// the loop routine runs over and over again forever:
void nnc_loop() {
    camera_fb_t *fb = NULL;
    fb = esp_camera_fb_get();

	unsigned char *fbp = (unsigned char *)fb->buf;
	for(int i;i<160*120; i++) fbp[i] = fbp[i*2+0]; // Compless YUYV(2B) to Grayscale(1B)
    memmove( fbp+15, fbp,160*120);
    memcpy( fbp, "P5 160 120 255\n", 15); // Set header

	nnc_main(fbp+15); // Call NNC

    esp_camera_fb_return(fb);
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

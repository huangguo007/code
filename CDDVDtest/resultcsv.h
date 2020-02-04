#define DllExport __declspec(dllexport)
#define DLLImport __declspec(dllimport)

#ifdef __cplusplus
extern "C" {
#endif

extern	DllExport int InitFile(char *);
extern	DllExport int AddData(char *, char *, char *, char *);

extern	DllExport int InitTotalFile(char *);
extern	DllExport FILE * OpenResult(char *);
extern	DllExport FILE * OpenTotalResult(char *);
extern	DllExport int CloseResult(FILE *);
extern	DllExport int CloseTotalResult(FILE *);
extern	DllExport int ReadData(char *, char *, char *, FILE *);
extern	DllExport int ReadTotalData(char *, char *, char *, char *, char *, char *, FILE *);
extern	DllExport int AddTotalResult(char *, char *, char *, char *, char *, char *, char *);

#ifdef __cplusplus
}
#endif

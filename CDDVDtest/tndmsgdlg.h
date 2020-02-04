#define DllExport __declspec(dllexport)
#define DLLImport __declspec(dllimport)

#ifdef __cplusplus
extern "C" {
#endif

extern	DllExport int tndMessageBox(LPCTSTR /* lpszText */ ,LPCTSTR /* lpszCaption */ ,UINT /* nType = 0 */);

#ifdef __cplusplus
}
#endif

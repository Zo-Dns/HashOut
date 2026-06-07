#define UNICODE
#define _UNICODE

#include <windows.h>
#include <wincrypt.h>
#include <shellapi.h>
#include <commctrl.h>
#include <stdio.h>
#include <wchar.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "comctl32.lib")

#define BUFSIZE 65536

// --------------------------------------------------------
// معرفات العناصر (IDs)
// --------------------------------------------------------
#define ID_BTN_MD5      1001
#define ID_BTN_SHA1     1002
#define ID_BTN_SHA256   1003
#define ID_BTN_OPEN     1004
#define ID_BTN_ABOUT    1005
#define ID_BTN_LANG     1006
#define ID_BTN_COMPARE  1007
#define ID_EDIT_COMPARE 1008
#define ID_BTN_SHA512   1009
#define ID_PROGRESS     1010
#define ID_BTN_VT       1011

#define WM_PROCESS_DONE     (WM_USER + 1)
#define WM_PROCESS_ERROR    (WM_USER + 2)
#define WM_PROCESS_PROGRESS (WM_USER + 3)

// =========================================================
// نظام اللغات (مصفوفة النصوص)
// =========================================================
typedef enum {
    STR_WINDOW_TITLE, STR_STATUS_IDLE, STR_STATUS_PROCESSING, STR_STATUS_DONE,
    STR_STATUS_COPIED_MD5, STR_STATUS_COPIED_SHA1, STR_STATUS_COPIED_SHA256, STR_STATUS_COPIED_SHA512,
    STR_BTN_OPEN, STR_BTN_ABOUT, STR_BTN_LANG, STR_BTN_COPY, STR_BTN_COMPARE,
    STR_COMPARE_PLACEHOLDER, STR_COMPARE_MATCH, STR_COMPARE_MISMATCH,
    STR_COMPARE_EMPTY_HASH, STR_COMPARE_EMPTY_INPUT, STR_OPEN_DIALOG_TITLE,
    STR_ERR_FILE, STR_ERR_MEM, STR_ERR_CRYPT, STR_ERR_HASH, STR_ERR_DATA, STR_ERR_RESULT,
    STR_ABOUT_TEXT, STR_ABOUT_TITLE,
    STR_BTN_VT, STR_STATUS_FOLDER, STR_STATUS_DONE_FOLDER, STR_ERR_EMPTY_HASH,
    STR_REPORT_MSG,
    STR_COUNT
} StringID;

const wchar_t* g_Strings[2][STR_COUNT] = {
    {   // ---- [0] English ----
        L"HashOut \u2014 Digital Fingerprint Extractor",
        L"Drag && drop a file or folder here...",
        L"\u23f3 Processing file, please wait...",
        L"\u2713 Done! You can copy the hash.",
        L"\u2713 MD5 copied.",
        L"\u2713 SHA-1 copied.",
        L"\u2713 SHA-256 copied.",
        L"\u2713 SHA-512 copied.",
        L"\U0001f4c2 Open",
        L"About",
        L"\u0639\u0631\u0628\u064a",
        L"Copy",
        L"Compare",
        L"Paste the developer's hash here to compare...",
        L"\u2705 MATCH \u2014 The file is authentic.",
        L"\u274c MISMATCH \u2014 File tampered!",
        L"Calculate a hash first.",
        L"Paste the hash in the box.",
        L"Select a file",
        L"Error: Could not open the file.",
        L"Error: Out of memory.",
        L"Error: Crypto engine failed.",
        L"Error: Hash objects failed.",
        L"Error: Data read failed.",
        L"Error: Hash extract failed.",
        L"HashOut v2.2\nDeveloper: Ahmed Al-Bayati\nSumer-Link \u00a9 2026",
        L"About HashOut",
        L"VirusTotal",
        L"\u23f3 Scanning Folder, generating report...",
        L"\u2713 Folder scan complete. Report saved!",
        L"Please extract a hash first.",
        L"Folder scan is complete!\n\nThe report has been saved at:\n%s"
    },
    {   // ---- [1] العربية ----
        L"HashOut \u2014 \u0627\u0633\u062a\u062e\u0631\u0627\u062c \u0627\u0644\u0628\u0635\u0645\u0629 \u0627\u0644\u0631\u0642\u0645\u064a\u0629",
        L"\u0627\u0633\u062d\u0628 \u0648\u0623\u0641\u0644\u062a \u0623\u064a \u0645\u0644\u0641 \u0623\u0648 \u0645\u062c\u0644\u062f \u0647\u0646\u0627...",
        L"\u23f3 \u062c\u0627\u0631\u064a \u0645\u0639\u0627\u0644\u062c\u0629 \u0627\u0644\u0645\u0644\u0641\u060c \u064a\u0631\u062c\u0649 \u0627\u0644\u0627\u0646\u062a\u0638\u0627\u0631...",
        L"\u2713 \u062a\u0645\u062a \u0627\u0644\u0645\u0639\u0627\u0644\u062c\u0629 \u0628\u0646\u062c\u0627\u062d.",
        L"\u2713 \u062a\u0645 \u0646\u0633\u062e MD5.",
        L"\u2713 \u062a\u0645 \u0646\u0633\u062e SHA-1.",
        L"\u2713 \u062a\u0645 \u0646\u0633\u062e SHA-256.",
        L"\u2713 \u062a\u0645 \u0646\u0633\u062e SHA-512.",
        L"\U0001f4c2 \u0641\u062a\u062d",
        L"\u062d\u0648\u0644",
        L"English",
        L"\u0646\u0633\u062e",
        L"\u0642\u0627\u0631\u0646",
        L"\u0627\u0644\u0635\u0642 \u0647\u0627\u0634 \u0627\u0644\u0645\u0628\u0631\u0645\u062c \u0647\u0646\u0627 \u0644\u0644\u0645\u0642\u0627\u0631\u0646\u0629...",
        L"\u2705 \u0645\u062a\u0637\u0627\u0628\u0642 \u2014 \u0627\u0644\u0645\u0644\u0641 \u0623\u0635\u0644\u064a.",
        L"\u274c \u063a\u064a\u0631 \u0645\u062a\u0637\u0627\u0628\u0642 \u2014 \u0642\u062f \u064a\u0643\u0648\u0646 \u0627\u0644\u0645\u0644\u0641 \u0645\u0639\u062f\u0644\u0627\u064b!",
        L"\u064a\u0631\u062c\u0649 \u062d\u0633\u0627\u0628 \u0627\u0644\u0647\u0627\u0634 \u0623\u0648\u0644\u0627\u064b.",
        L"\u064a\u0631\u062c\u0649 \u0644\u0635\u0642 \u0627\u0644\u0647\u0627\u0634 \u0641\u064a \u0627\u0644\u062e\u0627\u0646\u0629.",
        L"\u0627\u062e\u062a\u0631 \u0645\u0644\u0641\u0627\u064b",
        L"\u062e\u0637\u0623: \u062a\u0639\u0630\u0651\u0631 \u0641\u062a\u062d \u0627\u0644\u0645\u0644\u0641.",
        L"\u062e\u0637\u0623: \u0646\u0641\u0627\u0630 \u0627\u0644\u0630\u0627\u0643\u0631\u0629.",
        L"\u062e\u0637\u0623: \u0641\u0634\u0644 \u0645\u062d\u0631\u0643 \u0627\u0644\u062a\u0634\u0641\u064a\u0631.",
        L"\u062e\u0637\u0623: \u0641\u0634\u0644 \u0627\u0644\u0643\u0627\u0626\u0646\u0627\u062a.",
        L"\u062e\u0637\u0623: \u0641\u0634\u0644 \u0642\u0631\u0627\u0621\u0629 \u0627\u0644\u0628\u064a\u0627\u0646\u0627\u062a.",
        L"\u062e\u0637\u0623: \u0641\u0634\u0644 \u0627\u0644\u0627\u0633\u062a\u062e\u0631\u0627\u062c.",
        L"HashOut v2.2\n\u0627\u0644\u0645\u0637\u0648\u0631: \u0623\u062d\u0645\u062f \u0627\u0644\u0628\u064a\u0627\u062a\u064a\n\u0633\u0648\u0645\u0631-\u0644\u064a\u0646\u0643 \u00a9 2026",
        L"\u062d\u0648\u0644 HashOut",
        L"VirusTotal",
        L"\u23f3 \u062c\u0627\u0631\u064a \u0641\u062d\u0635 \u0627\u0644\u0645\u062c\u0644\u062f \u0648\u0627\u0633\u062a\u062e\u0631\u0627\u062c \u0627\u0644\u062a\u0642\u0631\u064a\u0631...",
        L"\u2713 \u062a\u0645 \u0641\u062d\u0635 \u0627\u0644\u0645\u062c\u0644\u062f \u0648\u062d\u0641\u0638 \u0627\u0644\u062a\u0642\u0631\u064a\u0631 \u0628\u0646\u062c\u0627\u062d!",
        L"\u064a\u0631\u062c\u0649 \u0627\u0633\u062a\u062e\u0631\u0627\u062c \u0627\u0644\u0647\u0627\u0634 \u0623\u0648\u0644\u0627\u064b.",
        L"\u0627\u0643\u062a\u0645\u0644 \u0641\u062d\u0635 \u0627\u0644\u0645\u062c\u0644\u062f \u0628\u0646\u062c\u0627\u062d!\n\n\u062a\u0645 \u062d\u0641\u0638 \u0627\u0644\u062a\u0642\u0631\u064a\u0631 \u0641\u064a \u0627\u0644\u0645\u0633\u0627\u0631 \u0627\u0644\u062a\u0627\u0644\u064a:\n%s"
    }
};

int g_Lang = 1; // الافتراضي عربي

#define S(id) g_Strings[g_Lang][id]

// --------------------------------------------------------
// المتغيرات العامة
// --------------------------------------------------------
HWND hMainWnd, hStatusText, hProgressBar;
HWND hEditMD5, hEditSHA1, hEditSHA256, hEditSHA512, hEditCompare;
HWND hBtnCompare, hBtnOpen, hBtnAbout, hBtnLang;
HWND hBtnCopyMD5, hBtnCopySHA1, hBtnCopySHA256, hBtnCopySHA512, hBtnVT;
HFONT hFont;
int g_CompareResult = 0;

typedef struct {
    wchar_t filename[MAX_PATH];
    BOOL isFolder;
} ThreadParam;


// ======================================================
//  نسخ النص إلى الحافظة
// ======================================================
void CopyToClipboard(HWND hwnd, HWND hEdit) {
    int len = GetWindowTextLengthW(hEdit);
    if (len == 0) return;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(wchar_t));
    if (!hMem) return;

    wchar_t* ptr = (wchar_t*)GlobalLock(hMem);
    if (!ptr) {
        GlobalFree(hMem);
        return;
    }

    GetWindowTextW(hEdit, ptr, len + 1);
    GlobalUnlock(hMem);

    if (OpenClipboard(hwnd)) {
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, hMem);
        CloseClipboard();
    } else {
        GlobalFree(hMem);
    }
}

// ======================================================
// دالة: تحويل بايتات إلى نص Hex
// ======================================================
void BytesToHexString(BYTE* hash, DWORD hashLen, wchar_t* outputStr) {
    for (DWORD i = 0; i < hashLen; i++) {
        swprintf(&outputStr[i * 2], 3, L"%02x", hash[i]);
    }
    outputStr[hashLen * 2] = L'\0';
}

// ======================================================
// دالة: المقارنة الذكية للهاش
// ======================================================
void DoCompare(HWND hwnd) {
    int inputLen = GetWindowTextLengthW(hEditCompare);

    if (inputLen == 0) {
        SetWindowTextW(hStatusText, S(STR_COMPARE_EMPTY_INPUT));
        g_CompareResult = 0;
        InvalidateRect(hStatusText, NULL, TRUE);
        return;
    }

    wchar_t* inputHash = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, (inputLen + 1) * sizeof(wchar_t));
    if (!inputHash) return;

    GetWindowTextW(hEditCompare, inputHash, inputLen + 1);

    for (int i = 0; inputHash[i]; i++) {
        inputHash[i] = towlower(inputHash[i]);
    }

    wchar_t* trimmed = inputHash;
    while (*trimmed == L' ') trimmed++;

    int tlen = wcslen(trimmed);
    while (tlen > 0 && trimmed[tlen-1] == L' ') {
        trimmed[--tlen] = L'\0';
    }

    int mLen = GetWindowTextLengthW(hEditMD5);
    if (mLen == 0) {
        SetWindowTextW(hStatusText, S(STR_COMPARE_EMPTY_HASH));
        g_CompareResult = 0;
        InvalidateRect(hStatusText, NULL, TRUE);
        HeapFree(GetProcessHeap(), 0, inputHash);
        return;
    }

    wchar_t hMD5[33]={0}, hSHA1[41]={0}, hSHA256[65]={0}, hSHA512[129]={0};
    GetWindowTextW(hEditMD5, hMD5, 33);
    GetWindowTextW(hEditSHA1, hSHA1, 41);
    GetWindowTextW(hEditSHA256, hSHA256, 65);
    GetWindowTextW(hEditSHA512, hSHA512, 129);

    BOOL matched = (wcscmp(trimmed, hMD5) == 0) ||
                   (wcscmp(trimmed, hSHA1) == 0) ||
                   (wcscmp(trimmed, hSHA256) == 0) ||
                   (wcscmp(trimmed, hSHA512) == 0);

    g_CompareResult = matched ? 1 : 2;
    SetWindowTextW(hStatusText, matched ? S(STR_COMPARE_MATCH) : S(STR_COMPARE_MISMATCH));
    InvalidateRect(hStatusText, NULL, TRUE);
    UpdateWindow(hwnd);
    HeapFree(GetProcessHeap(), 0, inputHash);
}

// ======================================================
//  فحص المجلدات واستخراج التقارير (يشمل المجلدات الفرعية)
// ======================================================
// دالة مساعدة تعاودية لفحص مجلد وكل مجلداته الفرعية
static void ScanDirectoryRecursive(FILE* f, HCRYPTPROV hProv, BYTE* buffer,
                                    const wchar_t* folderPath) {
    wchar_t searchPath[MAX_PATH];
    swprintf(searchPath, MAX_PATH, L"%s\\*", folderPath);

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchPath, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        // تجاهل "." و ".."
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
            continue;

        wchar_t fullPath[MAX_PATH];
        // التحقق من طول المسار قبل الدمج لتجنب الاقتطاع الصامت
        if (wcslen(folderPath) + wcslen(fd.cFileName) + 2 >= MAX_PATH)
            continue;
        swprintf(fullPath, MAX_PATH, L"%s\\%s", folderPath, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // مجلد فرعي → استدعاء تعاودي
            ScanDirectoryRecursive(f, hProv, buffer, fullPath);
        } else {
            // ملف → حساب الهاش
            HCRYPTHASH hMD5 = 0, hSHA256 = 0;
            HANDLE hFile = CreateFileW(fullPath, GENERIC_READ, FILE_SHARE_READ,
                                       NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
            if (hFile != INVALID_HANDLE_VALUE) {
                if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hMD5) &&
                    CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hSHA256)) {
                    DWORD cbRead;
                    while (ReadFile(hFile, buffer, BUFSIZE, &cbRead, NULL) && cbRead > 0) {
                        CryptHashData(hMD5, buffer, cbRead, 0);
                        CryptHashData(hSHA256, buffer, cbRead, 0);
                    }

                    BYTE md5Val[16], sha256Val[32];
                    DWORD len;
                    wchar_t hexMD5[33], hexSHA256[65];

                    len = 16; CryptGetHashParam(hMD5, HP_HASHVAL, md5Val, &len, 0);
                    BytesToHexString(md5Val, 16, hexMD5);
                    len = 32; CryptGetHashParam(hSHA256, HP_HASHVAL, sha256Val, &len, 0);
                    BytesToHexString(sha256Val, 32, hexSHA256);

                    fwprintf(f, L"File: %s\nMD5:  %s\nSHA256: %s\n\n",
                             fullPath, hexMD5, hexSHA256);
                }
                if (hMD5)    CryptDestroyHash(hMD5);
                if (hSHA256) CryptDestroyHash(hSHA256);
                CloseHandle(hFile);
            }
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
}

DWORD WINAPI ProcessFolderThread(LPVOID lpParam) {
    ThreadParam* param = (ThreadParam*)lpParam;
    wchar_t folderPath[MAX_PATH];
    wcscpy(folderPath, param->filename);
    HeapFree(GetProcessHeap(), 0, param);

    // مسار البرنامج الحالي
    wchar_t exeDir[MAX_PATH];
    GetModuleFileNameW(NULL, exeDir, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exeDir, L'\\');
    if (lastSlash) *lastSlash = L'\0';

    // اسم تسلسلي للتقرير
    wchar_t reportPath[MAX_PATH];
    int counter = 1;
    do {
        swprintf(reportPath, MAX_PATH, L"%s\\HashOut_Report_%d.txt", exeDir, counter);
        DWORD attr = GetFileAttributesW(reportPath);
        if (attr == INVALID_FILE_ATTRIBUTES) break;
        counter++;
    } while (counter < 10000);

    FILE* f = _wfopen(reportPath, L"w, ccs=UTF-8");
    if (!f) {
        PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_FILE, 0);
        return 1;
    }

    fwprintf(f, L"HashOut v2.2 - Forensic Folder Scan Report\n");
    fwprintf(f, L"Directory: %s\n", folderPath);
    fwprintf(f, L"--------------------------------------------------\n\n");

    HCRYPTPROV hProv = 0;
    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        BYTE* buffer = (BYTE*)HeapAlloc(GetProcessHeap(), 0, BUFSIZE);
        if (buffer) {
            ScanDirectoryRecursive(f, hProv, buffer, folderPath);
            HeapFree(GetProcessHeap(), 0, buffer);
        } else {
            // فشل تخصيص المخزن
            fclose(f);
            if (hProv) CryptReleaseContext(hProv, 0);
            PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_MEM, 0);
            return 1;
        }
        CryptReleaseContext(hProv, 0);
    }

    fclose(f);

    PostMessage(hMainWnd, WM_PROCESS_DONE, 2, 0);

    // إشعار + فتح التقرير
    wchar_t msgBoxText[MAX_PATH + 200];
    swprintf(msgBoxText, MAX_PATH + 200, S(STR_REPORT_MSG), reportPath);
    MessageBoxW(hMainWnd, msgBoxText, S(STR_ABOUT_TITLE), MB_OK | MB_ICONINFORMATION);

    ShellExecuteW(NULL, L"open", reportPath, NULL, NULL, SW_SHOWNORMAL);
    return 0;
}

// ======================================================
// دالة: معالجة ملف واحد (استخراج الهاش)
// ======================================================
DWORD WINAPI ProcessFileThread(LPVOID lpParam) {
    ThreadParam* param = (ThreadParam*)lpParam;
    wchar_t filename[MAX_PATH];
    wcscpy(filename, param->filename);
    HeapFree(GetProcessHeap(), 0, param);

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHashMD5 = 0, hHashSHA1 = 0, hHashSHA256 = 0, hHashSHA512 = 0;
    HANDLE hFile = NULL;
    BYTE* buffer = NULL;
    BOOL success = FALSE;

    hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_FILE, 0);
        return 1;
    }

    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;
    GetFileSizeEx(hFile, &fileSize);
    LONGLONG totalBytes = fileSize.QuadPart;
    LONGLONG bytesRead = 0;
    int lastPercent = 0;

    buffer = (BYTE*)HeapAlloc(GetProcessHeap(), 0, BUFSIZE);
    if (!buffer) {
        PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_MEM, 0);
        CloseHandle(hFile);
        return 1;
    }

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_CRYPT, 0);
        goto cleanup;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHashMD5) ||
        !CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHashSHA1) ||
        !CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHashSHA256) ||
        !CryptCreateHash(hProv, CALG_SHA_512, 0, 0, &hHashSHA512)) {
        PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_HASH, 0);
        goto cleanup;
    }

    DWORD cbRead = 0;
    while (ReadFile(hFile, buffer, BUFSIZE, &cbRead, NULL) && cbRead > 0) {
        if (!CryptHashData(hHashMD5, buffer, cbRead, 0) ||
            !CryptHashData(hHashSHA1, buffer, cbRead, 0) ||
            !CryptHashData(hHashSHA256, buffer, cbRead, 0) ||
            !CryptHashData(hHashSHA512, buffer, cbRead, 0)) {
            PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_DATA, 0);
            goto cleanup;
        }

        if (totalBytes > 0) {
            bytesRead += cbRead;
            int percent = (int)((bytesRead * 100) / totalBytes);
            if (percent != lastPercent) {
                lastPercent = percent;
                PostMessage(hMainWnd, WM_PROCESS_PROGRESS, (WPARAM)percent, 0);
            }
        }
    }

    static wchar_t hexMD5[33], hexSHA1[41], hexSHA256[65], hexSHA512[129];
    BYTE hashVal[64];
    DWORD hashLen;

    hashLen=16;
    if (!CryptGetHashParam(hHashMD5, HP_HASHVAL, hashVal, &hashLen, 0)) goto cleanup;
    BytesToHexString(hashVal, hashLen, hexMD5);
    SetWindowTextW(hEditMD5, hexMD5);

    hashLen=20;
    if (!CryptGetHashParam(hHashSHA1, HP_HASHVAL, hashVal, &hashLen, 0)) goto cleanup;
    BytesToHexString(hashVal, hashLen, hexSHA1);
    SetWindowTextW(hEditSHA1, hexSHA1);

    hashLen=32;
    if (!CryptGetHashParam(hHashSHA256, HP_HASHVAL, hashVal, &hashLen, 0)) goto cleanup;
    BytesToHexString(hashVal, hashLen, hexSHA256);
    SetWindowTextW(hEditSHA256, hexSHA256);

    hashLen=64;
    if (!CryptGetHashParam(hHashSHA512, HP_HASHVAL, hashVal, &hashLen, 0)) goto cleanup;
    BytesToHexString(hashVal, hashLen, hexSHA512);
    SetWindowTextW(hEditSHA512, hexSHA512);

    success = TRUE;
    PostMessage(hMainWnd, WM_PROCESS_DONE, 1, 0);

cleanup:
    if (hHashMD5) CryptDestroyHash(hHashMD5);
    if (hHashSHA1) CryptDestroyHash(hHashSHA1);
    if (hHashSHA256) CryptDestroyHash(hHashSHA256);
    if (hHashSHA512) CryptDestroyHash(hHashSHA512);
    if (hProv) CryptReleaseContext(hProv, 0);
    if (buffer) HeapFree(GetProcessHeap(), 0, buffer);
    if (hFile && hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

    if (!success) {
        PostMessage(hMainWnd, WM_PROCESS_ERROR, (WPARAM)STR_ERR_RESULT, 0);
    }

    return success ? 0 : 1;
}

// ======================================================
// دالة: التوجيه (للملفات أو المجلدات)
// ======================================================
void StartProcessing(LPCWSTR path, BOOL isFolder) {
    g_CompareResult = 0;
    SetWindowTextW(hEditMD5, L"");
    SetWindowTextW(hEditSHA1, L"");
    SetWindowTextW(hEditSHA256, L"");
    SetWindowTextW(hEditSHA512, L"");

    SetWindowTextW(hStatusText, isFolder ? S(STR_STATUS_FOLDER) : S(STR_STATUS_PROCESSING));
    SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
    ShowWindow(hProgressBar, isFolder ? SW_HIDE : SW_SHOW);
    InvalidateRect(hStatusText, NULL, TRUE);

    ThreadParam* param = (ThreadParam*)HeapAlloc(GetProcessHeap(), 0, sizeof(ThreadParam));
    if (!param) return;

    wcscpy(param->filename, path);
    HANDLE hThread = CreateThread(NULL, 0, isFolder ? ProcessFolderThread : ProcessFileThread, param, 0, NULL);
    if (hThread) CloseHandle(hThread);
}

// ======================================================
//  تحديث اللغة
// ======================================================
void UpdateUILanguage(HWND hwnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (g_Lang == 1) {
        exStyle |= WS_EX_LAYOUTRTL;
    } else {
        exStyle &= ~WS_EX_LAYOUTRTL;
    }
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    SetWindowTextW(hwnd, S(STR_WINDOW_TITLE));
    SetWindowTextW(hStatusText, S(STR_STATUS_IDLE));
    SetWindowTextW(hBtnOpen, S(STR_BTN_OPEN));
    SetWindowTextW(hBtnAbout, S(STR_BTN_ABOUT));
    SetWindowTextW(hBtnLang, S(STR_BTN_LANG));
    SetWindowTextW(hBtnCopyMD5, S(STR_BTN_COPY));
    SetWindowTextW(hBtnCopySHA1, S(STR_BTN_COPY));
    SetWindowTextW(hBtnCopySHA256, S(STR_BTN_COPY));
    SetWindowTextW(hBtnCopySHA512, S(STR_BTN_COPY));
    SetWindowTextW(hBtnCompare, S(STR_BTN_COMPARE));

    // تحديث نص placeholder (cue banner) — لا يمسح ما كتبه المستخدم
    SendMessageW(hEditCompare, EM_SETCUEBANNER, TRUE, (LPARAM)S(STR_COMPARE_PLACEHOLDER));

    // تحديث زر VirusTotal
    InvalidateRect(hBtnVT, NULL, TRUE);

    g_CompareResult = 0;
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

// ======================================================
// دالة معالجة الرسائل الرئيسية
// ======================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_CTLCOLORSTATIC: {
            HWND hwndCtrl = (HWND)lParam;
            if (hwndCtrl == hStatusText) {
                HDC hdc = (HDC)wParam;
                SetBkMode(hdc, TRANSPARENT);
                if (g_CompareResult == 1) {
                    SetTextColor(hdc, RGB(0, 140, 0));
                } else if (g_CompareResult == 2) {
                    SetTextColor(hdc, RGB(200, 0, 0));
                } else {
                    SetTextColor(hdc, RGB(60, 60, 60));
                }
                return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
            }
            break;
        }


        case WM_CREATE: {
            hMainWnd = hwnd;
            DragAcceptFiles(hwnd, TRUE);

            INITCOMMONCONTROLSEX icex = {0};
            icex.dwSize = sizeof(icex);
            icex.dwICC = ICC_PROGRESS_CLASS;
            InitCommonControlsEx(&icex);

            hFont = CreateFontW(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

            hStatusText = CreateWindowW(L"STATIC", S(STR_STATUS_IDLE), WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 12, 600, 22, hwnd, NULL, NULL, NULL);
            SendMessage(hStatusText, WM_SETFONT, (WPARAM)hFont, TRUE);

            hProgressBar = CreateWindowW(PROGRESS_CLASSW, NULL, WS_CHILD | PBS_SMOOTH, 20, 38, 600, 14, hwnd, (HMENU)ID_PROGRESS, NULL, NULL);
            SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);

            hBtnOpen = CreateWindowW(L"BUTTON", S(STR_BTN_OPEN), WS_VISIBLE | WS_CHILD, 20, 60, 175, 30, hwnd, (HMENU)ID_BTN_OPEN, NULL, NULL);
            hBtnAbout = CreateWindowW(L"BUTTON", S(STR_BTN_ABOUT), WS_VISIBLE | WS_CHILD, 205, 60, 110, 30, hwnd, (HMENU)ID_BTN_ABOUT, NULL, NULL);
            hBtnLang = CreateWindowW(L"BUTTON", S(STR_BTN_LANG), WS_VISIBLE | WS_CHILD, 325, 60, 110, 30, hwnd, (HMENU)ID_BTN_LANG, NULL, NULL);
            SendMessage(hBtnOpen, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnAbout, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnLang, WM_SETFONT, (WPARAM)hFont, TRUE);

            HWND lblMD5 = CreateWindowW(L"STATIC", L"MD5:", WS_VISIBLE | WS_CHILD, 20, 108, 55, 25, hwnd, NULL, NULL, NULL);
            hEditMD5 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 80, 105, 480, 26, hwnd, NULL, NULL, NULL);
            hBtnCopyMD5 = CreateWindowW(L"BUTTON", S(STR_BTN_COPY), WS_VISIBLE | WS_CHILD, 570, 104, 55, 28, hwnd, (HMENU)ID_BTN_MD5, NULL, NULL);

            HWND lblSHA1 = CreateWindowW(L"STATIC", L"SHA-1:", WS_VISIBLE | WS_CHILD, 20, 148, 55, 25, hwnd, NULL, NULL, NULL);
            hEditSHA1 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 80, 145, 480, 26, hwnd, NULL, NULL, NULL);
            hBtnCopySHA1 = CreateWindowW(L"BUTTON", S(STR_BTN_COPY), WS_VISIBLE | WS_CHILD, 570, 144, 55, 28, hwnd, (HMENU)ID_BTN_SHA1, NULL, NULL);

            HWND lblSHA256 = CreateWindowW(L"STATIC", L"SHA-256:", WS_VISIBLE | WS_CHILD, 20, 188, 70, 25, hwnd, NULL, NULL, NULL);
            hEditSHA256 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 95, 185, 380, 26, hwnd, NULL, NULL, NULL);
            hBtnCopySHA256 = CreateWindowW(L"BUTTON", S(STR_BTN_COPY), WS_VISIBLE | WS_CHILD, 485, 184, 60, 28, hwnd, (HMENU)ID_BTN_SHA256, NULL, NULL);

            hBtnVT = CreateWindowW(L"BUTTON", S(STR_BTN_VT), WS_VISIBLE | WS_CHILD, 550, 184, 75, 28, hwnd, (HMENU)ID_BTN_VT, NULL, NULL);
            HWND lblSHA512 = CreateWindowW(L"STATIC", L"SHA-512:", WS_VISIBLE | WS_CHILD, 20, 228, 70, 25, hwnd, NULL, NULL, NULL);
            hEditSHA512 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 95, 225, 465, 26, hwnd, NULL, NULL, NULL);
            hBtnCopySHA512 = CreateWindowW(L"BUTTON", S(STR_BTN_COPY), WS_VISIBLE | WS_CHILD, 570, 224, 55, 28, hwnd, (HMENU)ID_BTN_SHA512, NULL, NULL);

            HWND lblSep = CreateWindowW(L"STATIC", L"\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500", WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 260, 600, 15, hwnd, NULL, NULL, NULL);

            // مربع المقارنة — يبدأ فارغاً مع cue banner (placeholder حقيقي)
            hEditCompare = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 20, 282, 480, 28, hwnd, (HMENU)ID_EDIT_COMPARE, NULL, NULL);
            SendMessageW(hEditCompare, EM_SETCUEBANNER, TRUE, (LPARAM)S(STR_COMPARE_PLACEHOLDER));
            hBtnCompare = CreateWindowW(L"BUTTON", S(STR_BTN_COMPARE), WS_VISIBLE | WS_CHILD, 510, 281, 115, 30, hwnd, (HMENU)ID_BTN_COMPARE, NULL, NULL);

            SendMessage(hBtnVT, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(lblMD5, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditMD5, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnCopyMD5, WM_SETFONT, (WPARAM)hFont, TRUE);

            SendMessage(lblSHA1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditSHA1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnCopySHA1, WM_SETFONT, (WPARAM)hFont, TRUE);

            SendMessage(lblSHA256, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditSHA256, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnCopySHA256, WM_SETFONT, (WPARAM)hFont, TRUE);

            SendMessage(lblSHA512, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditSHA512, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnCopySHA512, WM_SETFONT, (WPARAM)hFont, TRUE);

            SendMessage(lblSep, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditCompare, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnCompare, WM_SETFONT, (WPARAM)hFont, TRUE);

            // ضبط الاتجاه واللغة مرة واحدة عند الإقلاع (يطبّق RTL إن كانت العربية افتراضية)
            UpdateUILanguage(hwnd);
            break;
        }

        case WM_PROCESS_PROGRESS:
            SendMessage(hProgressBar, PBM_SETPOS, wParam, 0);
            break;

        case WM_PROCESS_DONE:
            SendMessage(hProgressBar, PBM_SETPOS, 100, 0);
            ShowWindow(hProgressBar, SW_HIDE);
            g_CompareResult = 0;
            SetWindowTextW(hStatusText, wParam == 2 ? S(STR_STATUS_DONE_FOLDER) : S(STR_STATUS_DONE));
            InvalidateRect(hStatusText, NULL, TRUE);
            break;

        case WM_PROCESS_ERROR: {
            ShowWindow(hProgressBar, SW_HIDE);
            SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
            g_CompareResult = 0;
            SetWindowTextW(hStatusText, S((StringID)wParam));
            SetWindowTextW(hEditMD5, L"");
            SetWindowTextW(hEditSHA1, L"");
            SetWindowTextW(hEditSHA256, L"");
            SetWindowTextW(hEditSHA512, L"");
            InvalidateRect(hStatusText, NULL, TRUE);
            break;
        }

        case WM_COMMAND: {
            WORD id = LOWORD(wParam);

            if (id == ID_BTN_MD5) {
                CopyToClipboard(hwnd, hEditMD5);
                g_CompareResult=0;
                SetWindowTextW(hStatusText, S(STR_STATUS_COPIED_MD5));
                InvalidateRect(hStatusText, NULL, TRUE);
            }
            else if (id == ID_BTN_SHA1) {
                CopyToClipboard(hwnd, hEditSHA1);
                g_CompareResult=0;
                SetWindowTextW(hStatusText, S(STR_STATUS_COPIED_SHA1));
                InvalidateRect(hStatusText, NULL, TRUE);
            }
            else if (id == ID_BTN_SHA256) {
                CopyToClipboard(hwnd, hEditSHA256);
                g_CompareResult=0;
                SetWindowTextW(hStatusText, S(STR_STATUS_COPIED_SHA256));
                InvalidateRect(hStatusText, NULL, TRUE);
            }
            else if (id == ID_BTN_SHA512) {
                CopyToClipboard(hwnd, hEditSHA512);
                g_CompareResult=0;
                SetWindowTextW(hStatusText, S(STR_STATUS_COPIED_SHA512));
                InvalidateRect(hStatusText, NULL, TRUE);
            }
            else if (id == ID_BTN_VT) {
                wchar_t sha256[65] = {0};
                GetWindowTextW(hEditSHA256, sha256, 65);

                if (wcslen(sha256) == 64) {
                    wchar_t url[256];
                    swprintf(url, 256, L"https://www.virustotal.com/gui/file/%s", sha256);
                    ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
                } else {
                    MessageBoxW(hwnd, S(STR_ERR_EMPTY_HASH), L"Error", MB_OK | MB_ICONWARNING);
                }
            }
            else if (id == ID_BTN_OPEN) {
                OPENFILENAMEW ofn = {0};
                wchar_t szFile[MAX_PATH] = {0};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrTitle = S(STR_OPEN_DIALOG_TITLE);
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileNameW(&ofn)) {
                    StartProcessing(szFile, FALSE);
                }
            }
            else if (id == ID_BTN_ABOUT) {
                MessageBoxW(hwnd, S(STR_ABOUT_TEXT), S(STR_ABOUT_TITLE), MB_OK | MB_ICONINFORMATION);
            }
            else if (id == ID_BTN_LANG) {
                g_Lang = (g_Lang == 0) ? 1 : 0;
                UpdateUILanguage(hwnd);
            }
            else if (id == ID_BTN_COMPARE) {
                DoCompare(hwnd);
            }
            break;
        }

        case WM_DROPFILES: {
            HDROP hDropInfo = (HDROP)wParam;
            wchar_t szPath[MAX_PATH];

            DragQueryFileW(hDropInfo, 0, szPath, MAX_PATH);
            DragFinish(hDropInfo);

            DWORD attr = GetFileAttributesW(szPath);
            BOOL isFolder = (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
            StartProcessing(szPath, isFolder);
            break;
        }

        case WM_DESTROY:
            DeleteObject(hFont);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ======================================================
// نقطة الدخول الرئيسية
// ======================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszClassName = L"HashOutClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(1), IMAGE_ICON, 256, 256, LR_DEFAULTCOLOR);
    wc.hIconSm = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

    if (!RegisterClassExW(&wc)) return -1;

    HWND hwnd = CreateWindowW(
        L"HashOutClass",
        S(STR_WINDOW_TITLE),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        660, 375,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return -1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
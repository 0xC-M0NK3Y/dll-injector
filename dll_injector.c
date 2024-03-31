#include <stdio.h>

#include <windows.h>

int main(int argc, char **argv) {

    HANDLE th;
    HANDLE ph;
    void   *data;
    char   path[MAX_PATH+1] = {0};
    size_t path_size;
    size_t out;
    
    if (argc != 3) {
        printf("Usage: %s <pid> <.dll>\n", argv[0]);
        return 1;
    }
    path_size = GetFullPathNameA(argv[2], sizeof(path), path, NULL);
    if (path_size == 0) {
        printf("GetFullPathNameA failed: 0x%lX\n", GetLastError());
        return 1;
    }
    printf("Injecting %s : %s\n", argv[1], path);
    ph = OpenProcess(PROCESS_ALL_ACCESS, 0, atoi(argv[1]));
    if (ph == NULL) {
        printf("OpenProcess failed: 0x%lX\n", GetLastError());
        return 1;
    }
    data = VirtualAllocEx(ph, NULL, path_size + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (data == NULL) {
        printf("VirtualAllocEx failed: 0x%lX\n", GetLastError());
        return 1;
    }
    if (WriteProcessMemory(ph, data, path, path_size + 1, &out) == 0) {
        printf("WriteProcessMemory failed: 0x%lX\n", GetLastError());
        return 1;
    }
    th = CreateRemoteThread(ph, NULL, 0, (void *)LoadLibraryA, data, 0, NULL);
    if (th == NULL) {
        printf("CreateRemoteThread failed: 0x%lX\n", GetLastError());
        return 1;
    }
    printf("Injection successfull\n");
    return 0;
}

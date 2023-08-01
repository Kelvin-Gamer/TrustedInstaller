#include <Windows.h>
#include <Aclapi.h>
#include <iostream>

int main() {
    // Defina o nome do objeto cujo propriet�rio voc� deseja alterar
    LPCTSTR objectName = TEXT("C:\\test.txt");

    // Defina o novo propriet�rio
    LPCTSTR newOwner = TEXT("NT SERVICE\\TrustedInstaller");

    // Vari�veis para armazenar o SID do novo propriet�rio
    PSID pNewOwnerSID = nullptr;
    SID_NAME_USE sidType;

    // Obtenha o tamanho do buffer necess�rio para armazenar o SID
    DWORD dwSIDSize = 0;
    DWORD dwDomainNameSize = 0;
    LookupAccountName(nullptr, newOwner, pNewOwnerSID, &dwSIDSize, nullptr, &dwDomainNameSize, &sidType);

    // Aloque mem�ria para o SID
    pNewOwnerSID = (PSID)LocalAlloc(LPTR, dwSIDSize);

    // Obtenha o SID do novo propriet�rio
    TCHAR domainName[MAX_PATH];
    dwDomainNameSize = MAX_PATH;
    if (!LookupAccountName(nullptr, newOwner, pNewOwnerSID, &dwSIDSize, domainName, &dwDomainNameSize, &sidType)) {
        std::cerr << "Erro ao obter o SID do novo propriet�rio. C�digo de erro: " << GetLastError() << std::endl;
        LocalFree(pNewOwnerSID);
        return 1;
    }

    // Defina as informa��es de seguran�a para o objeto
    DWORD dwResult = SetNamedSecurityInfo(
        const_cast<LPTSTR>(objectName),
        SE_FILE_OBJECT,              // Tipo de objeto (neste exemplo, estamos usando um arquivo)
        OWNER_SECURITY_INFORMATION,  // Informa��es de seguran�a a serem definidas (propriet�rio)
        pNewOwnerSID,                // Novo SID do propriet�rio
        nullptr,                     // Grupo SID (n�o estamos alterando o grupo)
        nullptr,                     // Lista de controle de acesso (n�o estamos alterando ACL)
        nullptr                      // Lista de controle de auditoria (n�o estamos alterando as configura��es de auditoria)
    );

    // Libere a mem�ria alocada para o SID do novo propriet�rio
    if (pNewOwnerSID) {
        LocalFree(pNewOwnerSID);
    }

    if (dwResult == ERROR_SUCCESS) {
        std::cout << "O propriet�rio foi alterado com sucesso!" << std::endl;
    }
    else {
        std::cerr << "Erro ao alterar o propriet�rio. C�digo de erro: " << dwResult << std::endl;
    }

    return 0;
}

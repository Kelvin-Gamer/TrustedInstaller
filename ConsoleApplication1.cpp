#include <Windows.h>
#include <Aclapi.h>
#include <iostream>

int main() {
    // Defina o nome do objeto cujo proprietário você deseja alterar
    LPCTSTR objectName = TEXT("C:\\test.txt");

    // Defina o novo proprietário
    LPCTSTR newOwner = TEXT("NT SERVICE\\TrustedInstaller");

    // Variáveis para armazenar o SID do novo proprietário
    PSID pNewOwnerSID = nullptr;
    SID_NAME_USE sidType;

    // Obtenha o tamanho do buffer necessário para armazenar o SID
    DWORD dwSIDSize = 0;
    DWORD dwDomainNameSize = 0;
    LookupAccountName(nullptr, newOwner, pNewOwnerSID, &dwSIDSize, nullptr, &dwDomainNameSize, &sidType);

    // Aloque memória para o SID
    pNewOwnerSID = (PSID)LocalAlloc(LPTR, dwSIDSize);

    // Obtenha o SID do novo proprietário
    TCHAR domainName[MAX_PATH];
    dwDomainNameSize = MAX_PATH;
    if (!LookupAccountName(nullptr, newOwner, pNewOwnerSID, &dwSIDSize, domainName, &dwDomainNameSize, &sidType)) {
        std::cerr << "Erro ao obter o SID do novo proprietário. Código de erro: " << GetLastError() << std::endl;
        LocalFree(pNewOwnerSID);
        return 1;
    }

    // Defina as informações de segurança para o objeto
    DWORD dwResult = SetNamedSecurityInfo(
        const_cast<LPTSTR>(objectName),
        SE_FILE_OBJECT,              // Tipo de objeto (neste exemplo, estamos usando um arquivo)
        OWNER_SECURITY_INFORMATION,  // Informações de segurança a serem definidas (proprietário)
        pNewOwnerSID,                // Novo SID do proprietário
        nullptr,                     // Grupo SID (não estamos alterando o grupo)
        nullptr,                     // Lista de controle de acesso (não estamos alterando ACL)
        nullptr                      // Lista de controle de auditoria (não estamos alterando as configurações de auditoria)
    );

    // Libere a memória alocada para o SID do novo proprietário
    if (pNewOwnerSID) {
        LocalFree(pNewOwnerSID);
    }

    if (dwResult == ERROR_SUCCESS) {
        std::cout << "O proprietário foi alterado com sucesso!" << std::endl;
    }
    else {
        std::cerr << "Erro ao alterar o proprietário. Código de erro: " << dwResult << std::endl;
    }

    return 0;
}

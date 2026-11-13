# PSE em Ação

Versão 1.2. Código-fonte com mensagens em português e suporte à página de código UTF-8 no console do Windows.

Sistema acadêmico em linguagem C para apoiar o planejamento e o acompanhamento de ações coletivas do Programa Saúde na Escola (PSE).

## Funcionalidades

1. Cadastrar ações do PSE.
2. Listar ações cadastradas.
3. Pesquisar ações por código, escola ou tema.
4. Atualizar a situação (planejada, realizada ou cancelada) e registrar participantes efetivos.
5. Gerar resumo estatístico das ações e percentual de participação.
6. Registrar indicadores coletivos de bullying e acolhimento.
7. Consultar o painel de indicadores coletivos e a média de satisfação.

A opção 7 complementa o escopo mínimo do enunciado e mantém a temática definida na primeira entrega.

## Privacidade e limites

- São aceitos somente dados fictícios para demonstração acadêmica.
- O módulo de bullying registra somente totais agregados por escola e período.
- Não inserir nomes, identificadores de estudantes, relatos individuais, diagnósticos ou prontuários.
- O sistema não realiza diagnóstico, triagem, prescrição ou recomendação de tratamento.
- Os dados ficam em memória e são perdidos ao encerrar o programa.

## Como abrir no Dev-C++

1. Extraia a pasta do projeto em um local de fácil acesso.
2. Abra o Dev-C++.
3. Selecione **Arquivo > Novo > Projeto** e escolha **Console Application**; marque **C Project**.
4. Salve o projeto dentro da pasta `pse_em_acao`.
5. Adicione ou substitua o arquivo principal pelo `main.c` fornecido nesta pasta.
6. Em **Ferramentas > Opções do compilador**, confirme que o compilador GCC/MinGW está selecionado.
7. Compile e execute com **F11** (ou use Executar > Compilar e Executar, conforme a versão).

Se a versão do Dev-C++ permitir configurar parâmetros do compilador, utilize `-std=c99 -Wall -Wextra -pedantic`. O programa utiliza recursos compatíveis com C99.

## Compilação pelo terminal (GCC/MinGW)

No Windows, dentro da pasta do projeto:

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o pse_em_acao.exe
pse_em_acao.exe
```

## Regras de funcionamento

- Cada ação recebe um código numérico único.
- Campos textuais obrigatórios não podem ficar vazios.
- A data deve existir e seguir o formato `DD/MM/AAAA`.
- Quantidades devem ser números inteiros não negativos.
- Uma ação marcada como realizada solicita a quantidade efetiva de participantes.
- O percentual é calculado sobre as ações realizadas: participantes efetivos ÷ participantes previstos × 100.
- A média de satisfação coletiva é calculada pela soma das notas agregadas dividida pela quantidade de respostas. Cada resposta está na escala de 1 a 5.
- Limites de armazenamento: 100 ações e 100 registros de indicadores por execução.

## Estruturas utilizadas

- `struct Acao`: dados administrativos de cada ação do PSE.
- `struct IndicadorColetivo`: totais agregados por escola e período.
- Vetores estáticos para armazenar até 100 ações e 100 registros coletivos durante a execução.
- Funções separadas para cadastro, listagem, pesquisa, atualização, relatórios, leitura e validação.

## Engenharia de Software

Consulte `engenharia_software.md` para atores, escopo, requisitos atualizados, casos de uso, planejamento e plano de testes. O diagrama está disponível em `diagrama_casos_de_uso.png` e seu arquivo-fonte em `casos_de_uso.dot`.

## Plano de testes a executar pelo grupo

| ID | Cenário | Resultado esperado |
|---|---|---|
| T01 | Cadastrar ação com dados válidos | Ação cadastrada e código exibido |
| T02 | Repetir código já cadastrado | Sistema rejeita o código duplicado |
| T03 | Informar campo obrigatório vazio | Sistema solicita novo preenchimento |
| T04 | Informar data inexistente, como 31/02/2026 | Sistema rejeita a data |
| T05 | Listar ações | Todas as ações cadastradas são exibidas |
| T06 | Pesquisar por código, escola e tema | Registros correspondentes são exibidos |
| T07 | Pesquisar código inexistente | Mensagem de registro não encontrado |
| T08 | Atualizar ação para realizada | Solicita e registra participantes efetivos |
| T09 | Atualizar ação para cancelada | Situação muda para cancelada |
| T10 | Gerar resumo sem ações realizadas | Percentual informado como não calculável |
| T11 | Registrar indicadores agregados | Totais são armazenados sem dados individuais |
| T12 | Consultar painel de indicadores | Totais e média de satisfação são exibidos |
| T13 | Digitar opção fora do menu | Entrada rejeitada e menu permanece ativo |
| T14 | Cadastrar além do limite | Sistema informa limite de armazenamento |

**Importante:** os testes desta tabela são um roteiro. O grupo deve executá-los no Dev-C++ e registrar o resultado observado e uma captura de tela. Não marcar como aprovado antes da execução.

## Situação do projeto

Versão 1.2 do código-fonte. A compilação e os testes no ambiente Dev-C++ do grupo ainda precisam ser confirmados. O sistema não possui persistência em arquivos ou banco de dados.

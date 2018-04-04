# geratab
Programa para gerar combinações de passos.

Este programa foi criado com o propósito de gerar as tabelas de roscas para o meu torno Manrod MR301 (Sieg C3), tive a necessidade de gerar novas tabelas quando quis calcular os passos em polegadas com a incorporação de uma engrenagem de 63 dentes, o que permitiria melhores aproximações conforme artigo MEW204-P31_P36-Thread-Cutting.pdf em arceurotrade.co.uk 

Mas o uso do programa vai um pouco além, pois permite simular a adição de novas engrenagens à coleção e então verificar se compensa ou não.

Funciona para tornos onde os passos são calculados por:

P= K * pf ( NA / NB ) * ( NC / ND)

Onde:
K é o kludge factor(*1)
pf é o passo do fuso
NA, NB, NC e ND é o número de dentes das engrenagens A, B, C e D, respectivamente.

Os dados são informados por meio de arquivo texto de entrada. Nesse arquivo devem ser informados os números de dentes das engrenagens disponíveis, o passo do fuso e os passos que deseja pesquisar as combinações de engrenagens. Opcionalmente podem ser informado o Kludge-Factor(*1), o valor máximo da soma de dentes das engrenagens A e B, idem para C e D *(2).

O programa funciona por linha de comando, e deve ser executado da seguinte forma:

> geratab -f ArquivoDeEntrada.txt

Que gerará as tabelas em formato html conforme descrito no arquivo de entrada.

Se utilizar a opção -t conforme exemplo a seguir:

> geratab -t -f ArquivoDeEntrada.txt

Será gerada uma tabela extensa com todos os passos encontrados para o conjunto de engrenagens informada. 

Os arquivos de exemplo que começam com "manrod-MR301...", são os arquivos de entrada de exemplo. São uma boa fonte de consulta para entender como os dados devem ser agrupados.

Mas obedecendo as seguintes regras:
1- No arquivo de entrada não pode haver linhas em branco e nenhuma linha pode começar com espaço.
2- Caso o programa não seja o fornecido aqui no GitHub e seja compilado com outro compilador qua não o LCC-Win32 é necessário utilizar ponto no lugar de vírgula como separador decimal.
3- O programa é tosco e não verifica a correção do arquivo de entrada, assim se não respeitar a limitações aqui descritas o resultado provavelmente será incorreto.

O programa foi desenvolvido com o uso do ambiente LCC-Win32 (link: https://lcc-win32.services.net/), caso vá compilar em outro compilador que não o LCC-Win é necessário que edite o locale no código fonte, pois é esse locale que ambienta o programa para as entradas e saídas com o uso de vírgulas como separador decimal caso contrário os arquivos de entrada devem ser editados trocando as vírgulas por pontos.

Ricardo Maeda
rymaedaATyahoo.com

*(1) Kludge Factor é um multiplicador utilizado quando o trem de engrenagens não é acionado diretamente pelo eixo árvore. Nos tornos BV20, por exemplo, o Kludge é de 4,5.

*(2) As somas dos números de dentes diz respeito à distância entre eixos das respectivas engrenagens, limitar o valor máximo então seria por causa do limite físico para a montagem dessas engrenagens na grade do torno. Os valores do meu torno foram encontrados num grupo Yahoo de mini tornos.


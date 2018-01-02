# geratab
Programa para gerar combinações de passos.

Este programa foi criado com o propósito de gerar as tabelas de roscas para o meu torno Manrod MR301 (Sieg C3), tive a necessidade de gerar novas tabelas quando quis calcular os passos em polegadas com a incorporação de uma engrenagem de 63 dentes, o que permite melhores aproximações conforme artigo MEW204-P31_P36-Thread-Cutting.pdf em arceurotrade.co.uk 

Os dados são informados por meio de arquivo texto de entrada. Nesse arquivo devem ser informados os números de dentes das engrenagens disponíveis, o passo do fuso e os passos que deseja pesquisar as combinações de engrenagens. Opcionalmente podem ser informado o Kludge-Factor(1), o valor máximo da soma de dentes das engrenagens A e B, idem para C e D *(2).

Os arquivos de exemplo que começam com "manrod-MR301...", foram gerados para o programa compilado para Windows64, caso vá compilar em outro compilador que não o LCC-Win é necessário que edite os arquivos de exemplos trocando as vírgulas por pontos.

Ricardo Maeda
rymaedaATyahoo.com

*(1) Kludge Factor é um multiplicador utilizado quando o trem de engrenagens não é acionado diretamente pelo eixo árvore. Nos tornos BV20 o Kludge é de 4,5.

*(2) As somas dos números de dentes diz respeito à distância entre eixos das respectivas engrenagens, limitar o valor máximo então seria por causa do limite físico para a montagem dessas engrenagens na grade do torno. Os valores do meu torno foram encontrados num grupo Yahoo de mini tornos.

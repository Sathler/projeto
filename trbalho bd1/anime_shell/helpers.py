import datetime

def input_multiplas_linhas(msg=''):
    """Permite a entrada de múltiplas linhas."""

    texto = ''
    ultimas_linhas_vazias = 0
    print(msg)
    print('(Insira duas linhas vazias para sair.)', end='\n\n')
    while True:
        linha = input()
        if linha == '':
            if ultimas_linhas_vazias >= 1:
                break
            else:
                ultimas_linhas_vazias += 1
        else:
            linha += '\n'*ultimas_linhas_vazias
            ultimas_linhas_vazias = 0
        texto += linha + '\n'
    return texto

def eh_data_valida(data_str):
    """Valida datas no formato YYYY-MM-DD."""
    try:
        datetime.datetime.strptime(data_str, '%Y-%m-%d')
    except ValueError:
        return False
    return True


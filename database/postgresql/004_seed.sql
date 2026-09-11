INSERT INTO perfil (nome, descricao) VALUES
    ('USUARIO', 'Usuario da biblioteca'),
    ('BIBLIOTECARIO', 'Operador administrativo da biblioteca'),
    ('ADMINISTRADOR', 'Administrador do sistema')
ON CONFLICT (nome) DO NOTHING;

INSERT INTO configuracao (chave, valor, descricao) VALUES
    ('PRAZO_PADRAO_EMPRESTIMO_DIAS', '7', 'Prazo padrao de emprestimo em dias'),
    ('MAXIMO_EXEMPLARES_SIMULTANEOS', '5', 'Limite de exemplares simultaneos por usuario'),
    ('MAXIMO_RENOVACOES', '2', 'Limite de renovacoes por item de emprestimo')
ON CONFLICT (chave) DO NOTHING;

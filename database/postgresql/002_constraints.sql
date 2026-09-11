ALTER TABLE perfil ADD CONSTRAINT uq_perfil_nome UNIQUE (nome);
ALTER TABLE usuario ADD CONSTRAINT uq_usuario_cpf UNIQUE (cpf);
ALTER TABLE usuario ADD CONSTRAINT uq_usuario_email UNIQUE (email);
ALTER TABLE genero ADD CONSTRAINT uq_genero_nome UNIQUE (nome);
ALTER TABLE livro ADD CONSTRAINT uq_livro_isbn UNIQUE (isbn);
ALTER TABLE exemplar ADD CONSTRAINT uq_exemplar_codigo_barras UNIQUE (codigo_barras);
ALTER TABLE exemplar ADD CONSTRAINT uq_exemplar_rfid UNIQUE (rfid);
ALTER TABLE terminal_self_checkout ADD CONSTRAINT uq_terminal_codigo UNIQUE (codigo);
ALTER TABLE configuracao ADD CONSTRAINT uq_configuracao_chave UNIQUE (chave);

ALTER TABLE usuario ADD CONSTRAINT fk_usuario_perfil FOREIGN KEY (id_perfil) REFERENCES perfil (id_perfil);
ALTER TABLE livro ADD CONSTRAINT fk_livro_editora FOREIGN KEY (id_editora) REFERENCES editora (id_editora);
ALTER TABLE livro_autor ADD CONSTRAINT fk_livro_autor_livro FOREIGN KEY (id_livro) REFERENCES livro (id_livro) ON DELETE CASCADE;
ALTER TABLE livro_autor ADD CONSTRAINT fk_livro_autor_autor FOREIGN KEY (id_autor) REFERENCES autor (id_autor) ON DELETE CASCADE;
ALTER TABLE livro_genero ADD CONSTRAINT fk_livro_genero_livro FOREIGN KEY (id_livro) REFERENCES livro (id_livro) ON DELETE CASCADE;
ALTER TABLE livro_genero ADD CONSTRAINT fk_livro_genero_genero FOREIGN KEY (id_genero) REFERENCES genero (id_genero) ON DELETE CASCADE;
ALTER TABLE exemplar ADD CONSTRAINT fk_exemplar_livro FOREIGN KEY (id_livro) REFERENCES livro (id_livro);
ALTER TABLE emprestimo ADD CONSTRAINT fk_emprestimo_usuario FOREIGN KEY (id_usuario) REFERENCES usuario (id_usuario);
ALTER TABLE emprestimo_item ADD CONSTRAINT fk_emprestimo_item_emprestimo FOREIGN KEY (id_emprestimo) REFERENCES emprestimo (id_emprestimo) ON DELETE CASCADE;
ALTER TABLE emprestimo_item ADD CONSTRAINT fk_emprestimo_item_exemplar FOREIGN KEY (id_exemplar) REFERENCES exemplar (id_exemplar);
ALTER TABLE reserva ADD CONSTRAINT fk_reserva_usuario FOREIGN KEY (id_usuario) REFERENCES usuario (id_usuario);
ALTER TABLE reserva ADD CONSTRAINT fk_reserva_livro FOREIGN KEY (id_livro) REFERENCES livro (id_livro);

ALTER TABLE exemplar ADD CONSTRAINT ck_exemplar_status CHECK (status IN ('DISPONIVEL', 'EMPRESTADO', 'RESERVADO', 'MANUTENCAO', 'EXTRAVIADO'));
ALTER TABLE emprestimo ADD CONSTRAINT ck_emprestimo_status CHECK (status IN ('ABERTO', 'ENCERRADO', 'CANCELADO'));
ALTER TABLE emprestimo ADD CONSTRAINT ck_emprestimo_origem CHECK (origem IN ('BALCAO', 'SELF_CHECKOUT'));
ALTER TABLE emprestimo_item ADD CONSTRAINT ck_emprestimo_item_renovacoes CHECK (quantidade_renovacoes >= 0);
ALTER TABLE reserva ADD CONSTRAINT ck_reserva_status CHECK (status IN ('ATIVA', 'ATENDIDA', 'CANCELADA', 'EXPIRADA'));

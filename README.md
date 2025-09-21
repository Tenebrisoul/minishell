# Minishell

Bu proje, bash shell'inin temel özelliklerini yeniden implemente eden bir mini shell uygulamasıdır. 42 okulu eğitim programının bir parçası olarak geliştirilmiştir.

## Proje Yapısı

```
minishell/
├── core/              # Ana shell çekirdeği
├── lexer/             # Leksikal analiz (tokenization)
├── parser/            # Sözdizimi analizi (AST oluşturma)
├── executor/          # Komut çalıştırma modülü
├── builtin/           # Dahili komutlar
├── expand/            # Değişken genişletme
├── environment/       # Çevre değişkenleri yönetimi
├── signals/           # Sinyal işleme
├── utils/             # Yardımcı fonksiyonlar
├── libft/             # Özel string/utility fonksiyonları
├── gc/                # Bellek yönetimi (Garbage Collector)
└── minishell.h        # Ana header dosyası
```

## Nasıl Çalışır?

### 1. Program Akışı

Minishell şu adımları takip eder:

1. **Başlatma** (`core/main.c:27`):
   - `init_env()`: Çevre değişkenlerini yükler
   - `init_signals()`: Sinyal işleyicilerini ayarlar
   - `shell_run()`: Ana REPL döngüsünü başlatır

2. **REPL Döngüsü** (`core/repl.c:25`):
   ```
   Döngü:
   ├── Kullanıcı girdisini al (readline)
   ├── Girdiye tokenize et (lexer)
   ├── AST oluştur (parser)
   ├── Komutları çalıştır (executor)
   └── Sonucu göster
   ```

### 2. Modüller ve Fonksiyonları

#### **Lexer Modülü** (`lexer/`)
- **Amaç**: Kullanıcı girdisini anlamlı tokenlara böler
- **Ana fonksiyon**: `lexer()` (`lexer/lexer.c:42`)
- **Token türleri**:
  - `TOKEN_WORD`: Kelimeler ve komutlar
  - `TOKEN_PIPE`: `|` (pipe)
  - `TOKEN_REDIR_IN`: `<` (input redirection)
  - `TOKEN_REDIR_OUT`: `>` (output redirection)
  - `TOKEN_REDIR_APPEND`: `>>` (append redirection)
  - `TOKEN_HEREDOC`: `<<` (heredoc)
  - `TOKEN_SEMICOLON`: `;` (komut ayırıcı)

**Örnek**:
```bash
echo "hello" | grep h > output.txt
# Tokenize edilir:
# [TOKEN_WORD: echo] [TOKEN_WORD: "hello"] [TOKEN_PIPE: |] [TOKEN_WORD: grep] [TOKEN_WORD: h] [TOKEN_REDIR_OUT: >] [TOKEN_WORD: output.txt]
```

#### **Parser Modülü** (`parser/`)
- **Amaç**: Tokenleri Abstract Syntax Tree (AST) yapısına dönüştürür
- **Ana fonksiyon**: `parser()` (`parser/parser.c:59`)
- **AST düğüm türleri**:
  - `NODE_COMMAND`: Tek komut
  - `NODE_PIPELINE`: Pipe ile bağlı komutlar
  - **NODE_SEQUENCE**: `;` ile ayrılmış komutlar

**Parser hiyerarşisi**:
```
parse_sequence() (en üst seviye)
└── parse_pipeline()
    └── parse_ast_node() (komut + yönlendirmeler)
```

#### **Executor Modülü** (`executor/`)
- **Amaç**: AST'yi yorumlar ve komutları çalıştırır
- **Ana fonksiyon**: `exec_ast()` (`executor/executor.c:49`)
- **Çalıştırma türleri**:
  - **Dahili komutlar**: `is_builtin()` kontrolü (`builtin/builtins.c:3`)
  - **Harici komutlar**: `fork()` ve `execve()` ile
  - **Pipeline**: Pipe'lar arası iletişim
  - **Sequence**: Komutları sırayla çalıştır

#### **Built-in Komutlar** (`builtin/`)
Desteklenen dahili komutlar:
- `echo`: Metin yazdırma (`builtin/ms_echo.c`)
- `cd`: Dizin değiştirme (`builtin/ms_cd.c`)
- `pwd`: Mevcut dizini göster (`builtin/ms_pwd.c`)
- `export`: Çevre değişkeni tanımla (`builtin/ms_env.c`)
- `unset`: Çevre değişkenini sil (`builtin/ms_env.c`)
- `env`: Çevre değişkenlerini listele (`builtin/ms_env.c`)
- `exit`: Shell'den çık (`builtin/ms_exit.c`)

#### **Expand Modülü** (`expand/`)
- **Amaç**: Değişken genişletme ve quote işleme
- **Ana fonksiyon**: `expand()` (`expand/expand_main.c`)
- **Özellikler**:
  - `$VARIABLE` genişletme
  - `$?` (exit status) genişletme
  - Quote işleme (`"` ve `'`)

**Örnek**:
```bash
echo "Hello $USER"  # "Hello root" olur
echo '$USER'        # '$USER' olarak kalır (tek tırnak)
```

#### **Environment Modülü** (`environment/`)
- **Amaç**: Çevre değişkenlerini yönetir
- **Veri yapısı**: Bağlı liste (`t_env_item`)
- **Ana fonksiyonlar**:
  - `get_env()`: Environment instance'ını al (`environment/env_init.c:30`)
  - `add_env_item()`: Yeni değişken ekle
  - `unset_env_item()`: Değişken sil

#### **Garbage Collector** (`gc/`)
- **Amaç**: Bellek sızıntılarını önler
- **Fonksiyonlar**:
  - `alloc()`: GC ile yönetilen malloc (`gc/gc.c`)
  - `dump_gc()`: Tüm ayrılan belleği temizle

### 3. Sinyal İşleme (`signals/`)

- **SIGINT (Ctrl+C)**: Mevcut komutu kesintiye uğratır
- **SIGQUIT (Ctrl+\)**: Ignore edilir
- **State yönetimi**:
  - `STATE_NORMAL`: Normal prompt
  - `STATE_COMMAND`: Komut çalışırken
  - `STATE_HEREDOC`: Heredoc girişi sırasında

### 4. Heredoc İşleme (`core/heredoc*.c`)

Heredoc (`<<`) operatörü için özel işleme:
```bash
cat << EOF
Bu bir heredoc
Birden fazla satır
EOF
```

- **Ana fonksiyon**: `read_heredoc_input()` (`core/heredoc_input.c`)
- **Özellikler**:
  - Delimiter kontrolü
  - Değişken genişletme (quote edilmemişse)
  - Sinyal handling

## Derleme ve Çalıştırma

```bash
# Derleme
make

# Çalıştırma
./minishell

# Temizlik
make clean      # Object dosyalarını sil
make fclean     # Tüm oluşturulan dosyaları sil
make re         # Yeniden derle
```

## Desteklenen Özellikler

### ✅ Temel Özellikler
- Komut çalıştırma ve PATH arama
- Dahili komutlar (echo, cd, pwd, export, unset, env, exit)
- Input/Output yönlendirme (`<`, `>`, `>>`)
- Pipe (`|`) operatörü
- Çevre değişkeni genişletme (`$VAR`, `$?`)
- Quote işleme (`"` ve `'`)
- Heredoc (`<<`)
- Sinyal işleme (Ctrl+C, Ctrl+\)

### ✅ Gelişmiş Özellikler
- History (readline kütüphanesi)
- Tab completion (readline)
- Exit status yönetimi
- Bellek yönetimi (Garbage Collector)
- Error handling

## Kod Mimarisi

### Veri Yapıları

1. **Token** (`minishell.h:77`):
```c
typedef struct s_token {
    t_token_type type;
    char *value;
    struct s_token *next;
} t_token;
```

2. **AST Node** (`minishell.h:107`):
```c
typedef struct s_ast_node {
    t_node_type type;
    union {
        t_command *command;
        struct {
            struct s_ast_node *left;
            struct s_ast_node *right;
        } s_binary;
    } u_data;
} t_ast_node;
```

3. **Environment** (`minishell.h:134`):
```c
typedef struct s_env {
    t_env_item *first_node;
    int exit_status;
} t_env;
```

### Execution Flow

```
Kullanıcı girdisi → Lexer → Parser → Executor
                     ↓        ↓        ↓
                  Tokens    AST    Komut çalıştırma
```

Bu minishell implementasyonu, modüler bir tasarım kullanarak bash'in temel özelliklerini başarıyla taklit eder ve güvenli bellek yönetimi sağlar.
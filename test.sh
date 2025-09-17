#!/bin/bash

# Minishell Test Runner
# Bu script'i çalıştırmak için: ./test.sh

echo "🚀 Minishell Test Suite"
echo "======================"

# Test dosyalarının varlığını kontrol et
if [ ! -f "test_minishell.py" ]; then
    echo "❌ test_minishell.py bulunamadı!"
    exit 1
fi

if [ ! -f "test_cases_enhanced.json" ]; then
    echo "❌ test_cases_enhanced.json bulunamadı!"
    exit 1
fi

if [ ! -f "minishell" ]; then
    echo "📦 Minishell'i build ediliyor..."
    make
    if [ $? -ne 0 ]; then
        echo "❌ Build başarısız!"
        exit 1
    fi
fi

# Test kategorisini kontrol et
if [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
    echo "Kullanım:"
    echo "  ./test.sh                    # Tüm testleri çalıştır"
    echo "  ./test.sh echo              # Sadece echo testlerini çalıştır"
    echo "  ./test.sh --list            # Test kategorilerini listele"
    echo "  ./test.sh echo --verbose    # Echo testlerini verbose modda çalıştır"
    echo ""
    echo "Mevcut kategoriler:"
    python3 test_minishell.py ./minishell --json-file test_cases_enhanced.json --list-categories 2>/dev/null | grep -E "^\s+[a-z]" || echo "  echo, cd, pwd, env, export, unset, exit, pipes, redirections, variables, quotes, errors"
    exit 0
fi

if [ "$1" == "--list" ]; then
    echo "📋 Mevcut test kategorileri:"
    python3 test_minishell.py ./minishell --json-file test_cases_enhanced.json --list-categories
    exit 0
fi

# Test komutunu oluştur
CMD="python3 /root/Desktop/minishell/test_minishell.py /root/Desktop/minishell/minishell --json-file /root/Desktop/minishell/test_cases_enhanced.json --timeout 10"

if [ ! -z "$1" ] && [ "$1" != "--verbose" ] && [ "$1" != "-v" ]; then
    CMD="$CMD --category $1"
fi

if [ "$2" == "--verbose" ] || [ "$2" == "-v" ] || [ "$1" == "--verbose" ] || [ "$1" == "-v" ]; then
    CMD="$CMD --verbose"
fi

echo "🔄 Test komutu: $CMD"
echo ""

# Testleri çalıştır
$CMD
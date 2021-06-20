#!/usr/bin/env python3
# coding: utf8
import os
import glob


def enumerate_content_sources() -> list[str]:
    return [os.path.abspath(path) for path in glob.glob('data/*.txt')]


def extract_words_from_file(path: str) -> list[str]:
    with open(path, 'r') as file:
        return list(map(lambda word: word.lower() + os.linesep,
                        list(filter(
                            lambda word: len(word) > 3 and word.isalnum(),
                            file.read().split(' ')))))


if __name__ == '__main__':
    all_words = set()
    for content_source in enumerate_content_sources():
        print(f'processing {content_source}')
        words = extract_words_from_file(content_source)
        print(f'\tobtained {len(words)} word(s)')
        all_words |= set(words)

    print(f'obtained a total of {len(all_words)} unique word(s)')
    with open('data/dictionary.list', 'w') as file:
        file.writelines(sorted(all_words))

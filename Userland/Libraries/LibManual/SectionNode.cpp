/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SectionNode.h"
#include "PageNode.h"
#include <AK/LexicalPath.h>
#include <AK/QuickSort.h>
#include <LibCore/DirIterator.h>

namespace Manual {

ErrorOr<String> SectionNode::path() const
{
    return String::formatted("/usr/share/man/man{}", m_section);
}

ErrorOr<String> SectionNode::name() const
{
    return String::formatted("{}. {}", m_section, m_name);
}

ErrorOr<void> SectionNode::reify_if_needed() const
{
    if (m_reified)
        return {};
    m_reified = true;

    Core::DirIterator dir_iter { TRY(path()).to_deprecated_string(), Core::DirIterator::Flags::SkipDots };

    Vector<String> page_names;
    while (dir_iter.has_next()) {
        LexicalPath lexical_path(dir_iter.next_path());
        if (lexical_path.extension() != "md")
            continue;
        page_names.append(TRY(String::from_utf8(lexical_path.title())));
    }

    quick_sort(page_names);

    for (auto& page_name : page_names)
        m_children.append(TRY(try_make_ref_counted<PageNode>(*this, move(page_name))));

    return {};
}

void SectionNode::set_open(bool open)
{
    if (m_open == open)
        return;
    m_open = open;
}

Array<NonnullRefPtr<SectionNode>, number_of_sections> const sections = { {
    make_ref_counted<SectionNode>("1"sv, "User Programs"sv),
    make_ref_counted<SectionNode>("2"sv, "System Calls"sv),
    make_ref_counted<SectionNode>("3"sv, "Library Functions"sv),
    make_ref_counted<SectionNode>("4"sv, "Special Files"sv),
    make_ref_counted<SectionNode>("5"sv, "File Formats"sv),
    make_ref_counted<SectionNode>("6"sv, "Games"sv),
    make_ref_counted<SectionNode>("7"sv, "Miscellanea"sv),
    make_ref_counted<SectionNode>("8"sv, "Sysadmin Tools"sv),
} };

}

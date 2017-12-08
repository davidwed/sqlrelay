#!/usr/bin/tclsh
# $Id: tcldoc.tcl,v 1.1 2012/01/05 22:36:52 mused Exp $

#//#
# TclDoc parses the declarations and documentation comments in a set
# of Tcl source files and produces a corresponding set of HTML pages
# describing procedure declarations.  Run TclDoc on a set of files
# and/or directories.  It builds a rich internal representation of the
# files, determining both procedure declarations and procedure
# calls. TclDoc will run on .tcl and .tsh source files that are pure
# stub files with no procedure bodies.  This means you can write
# documentation comments and run TclDoc in the earliest stages of
# design while creating the API, before writing the implementation.
#
# @author Jason Tang (tang@jtang.org)
# @version 1.0
# @see http://mini.net/tcl/TclDoc
#//#

source "[file join [file dirname $argv0] tcldoc_scanner.tcl]"
set TCLDOC_VERSION 0.3

######################################################################
# main TclDoc functions

# Initialize all of the various cross-reference tables used by TclDoc.
# If an import record was given then merge that record with these
# tables.
proc initialize_tables {} {
    set ::all_files ""
    foreach c { : 1 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z } {
        set ::toc_table($c) 0
    }
}

# Creates the destination directory as necessary.  Copy over the
# overview file (if <code>--overview</code> specified) and doc files
# (<code>--doc-files</code>) as necessary.
proc prepare_destination {} {
    # create the destination directory if needed
    print_status "Writing to directory $::dest_dir."
    if [file exists $::dest_dir] {
        if {![file isdir $::dest_dir]} {
            tcldoc_error "Destination is not a directory." $::IO_ERROR
        }
        if {!$::force_overwrite} {
            puts -nonewline "Warning: Destination already exists.  Proceed? "
            flush stdout
            gets stdin in
            if {![string equal -length 1 -nocase $in "y"]} {
                exit
            }
        }
    } elseif [catch {file mkdir $::dest_dir}] {
        tcldoc_error "Could not create directory $::dest_dir." $::IO_ERROR
    }
    if {$::doc_dir != ""} {
        print_status "Copying doc files..."
        foreach doc $::doc_dir {
            if {[file exists $doc] && [file isfile $doc]} {
                # copy invidiual file
                file copy -force -- $doc $::dest_dir
            } elseif {[file exists $doc] && [file isdirectory $doc]} {
                # copy entire directory
                set doc_dest_dir [file join $::dest_dir [file tail $doc]]
                if {![file exists $doc_dest_dir]} {
                    if [catch {file mkdir $doc_dest_dir}] {
                        tcldoc_error "Could not create $doc_dest_dir for doc-files." $::IO_ERROR
                    }
                }
                foreach doc_file [glob -directory $doc *] {
                    file copy -force -- $doc_file $doc_dest_dir
                }
            } else {
                tcldoc_error puts stderr "Invalid doc-files file or directory $doc." $::IO_ERROR
            }
        }
    }
    if {$::overview_file != ""} {
        print_status "Copying overview file..."
        if {[file exists $::overview_file] && [file isfile $::overview_file]} {
            if [catch {file copy -force $::overview_file $::dest_dir}] {
                tcldoc_error "Could not copy overview file $::overview_file." $::IO_ERROR
            }
        } else {
            tcldoc_error "Invalid overview file: $::overview_file." $::IO_ERROR
        }
    }
}


proc write_export_file {} {
}

######################################################################
# functions affecting individual files

# Scans a file for all instances of lines beginning with
# <code>proc</code> indicating a procedure declaration.  Add
# discovered declarations to the procedure table along with its line
# number in the file.
#
# @param filename file to scan for procedure declarations
proc declaration_scan {filename} {
    set basename [file tail $filename]
    set newhtmlname "${basename}.html"
    print_status "Scanning $filename:"
    if [catch {open $filename r} src] {
        puts stderr "  Unable to open $filename -- skipping."
        break
    }
    set ::current_file $filename
    set ::line_number 0
    while {[gets $src line] >= 0} {
        incr ::line_number
        if [regexp {\A\s*proc\s+([^\s\{]+)} $line foo procname] {
            # add the procedure and line number to the file index
            lappend ::file_table($basename) [list $procname $::line_number]
            # add the filename and line number to the procedure index
            lappend ::proc_table($procname) [list $basename $::line_number]
            print_status "  $procname"
        }
    }
    close $src
}

# Take a single source Tcl file and scan it intensively, generating
# its HTML version.  Identify comment blocks and highlight them in the
# HTML version.  If the comment is a procedure-level or file-level
# comment then pipe it through the scanner for annotation purposes.
# Identify procedure declarations, add &lt;a name&gt; anchors and add
# their parameters to the function table.  Identify procedure calls
# and add &lt;a href&gt; hypertext.  Substitute proper html codes for
# special symbols <, >, &, and &#34;.  Write the HTML marked version
# as well as the annotations.
#
# @param filename file to scan
# @see scan_recursively
proc deep_scan {filename} {
    set basename [file tail $filename]
    set htmlname "${basename}.html"
    set annothtmlname "${basename}-annot.html"
    set txtname "${basename}.txt"
    print_status "Building $htmlname..."
    
    # figure out my "docroot" path
    # *** FIX ME ***: until the notion of 'packages' is added
    # here, docroot will be set to nothing
    set docroot "."
    
    # read the entire source file into memory
    if [catch {open $filename r} src] {
        print_status "  Unable to open source file -- skipping."
        return
    }
    set srcbuf [read $src]
    close $src

    # start writing HTML version
    if [catch {open [file join $::dest_dir $htmlname] w} dest] {
        print_status "  Unable to create destination file -- skipping."
        return
    }
    write_header $dest $basename $basename
    puts $dest "<strong>$basename</strong>
\(<a href=\"$annothtmlname\">annotations</a> | <a href=\"$txtname\">original source</a>\)
<p>
<pre>"    

    # start writing the annotated file
    if [catch {open [file join $::dest_dir $annothtmlname] w} annot] {
        print_status "  Unable to create annotation file -- skipping."
        close $dest
        return
    }
    write_header $annot $basename "$basename annotations"
    puts $annot "<h2><a href=\"$htmlname\">$basename</a> Annotations</h2>"
    if {!$::hide_paths} {
        puts $annot "Created from <strong><code>[sanitize [file nativename $filename]]</strong></code>"
    }
    puts $annot "<hr>"
    new_annotation $annot $basename $annothtmlname $docroot
    
    set ::current_file $filename
    set ::line_number 1

    scan_recursively $dest $srcbuf $basename $annothtmlname

    # add this file to the summary table
    if $::hide_paths {
        set sourceloc ""
    } else {
        set sourceloc "[file nativename [file dirname $filename]]"
    }
    add_summary $basename \
        $annothtmlname "" $sourceloc $::annotfile(file_summary) file

    puts $dest "</pre>"
    write_footer $dest
    close $dest
    write_annotation
    write_footer $annot
    close $annot
}

# Given a buffer of Tcl code recursively examine each command within.
# Commands follow normal Tcl syntax -- they are either terminated by
# newlines or semicolons.  If a single command has multiple parts
# (such as an <code>if</code> statement) recursively examine each
# subpart.  In this way discover comment blocks, procedure
# declarations, and procedure calls.
# <p>
# There are limits to this scanner because it only does static
# analysis.  Mainly, things that make Tcl such a dynamic language
# (such as <code>eval</code> and <code>subst</code> commands) may
# potentially confuse this scanner.
#
# @param buffer buffer of Tcl code to examine
# @param basename source file from which this Tcl code originated
# @param dest I/O channel to write HTML-ized version of the buffer
proc scan_recursively {dest buffer basename annotname} {
    set comment_block ""
    while {$buffer != ""} {
        set output ""
        set line_complete 0
        set line ""
        # consume whitespace
        if [regexp -- {\A(\s+)(.*)} $buffer foo match buffer] {
            append output $match
            # file and procedure annoations must be contiguous; if
            # there are any newlines between then stop the block
            if {[string first "\n" $match] >= 0} {
                set comment_block ""
            }
        }
        # grab the next "command" from the src buffer
        while {!$line_complete && $buffer != ""} {
            if [regexp -- {\A([^;\n]+)(.*)} $buffer foo l buffer] {
                append line $l
            }
            # there are two special cases: the original line began
            # with a hash, in which I should consume the rest of the
            # line, or if $l ended with a backslash
            if {[string index $line 0] == "\#"} {
                # note how this will consume semicolons within the
                # comment
                regexp -- {\A([^\n]*)(.*)} $buffer foo l buffer
                append line $l
                set line_complete 1
            } elseif {![info complete $line] || [string index $line end] == "\\"} {
                append line [string index $buffer 0]
                set buffer [string range $buffer 1 end]
            } else {
                set line_complete 1
            }
        }
        # apparantly Tcl allows trailing bare backslashes, so make a
        # special check here
        if {[string index [string trimright $line] end] == "\\" && \
                [string length $buffer] == 0} {
            set line_complete 1
        }
        if {!$line_complete && $output == ""} {
            tcldoc_file_warning "Source does not appear to be valid Tcl code, skipping"
            write_and_update $dest $line
            continue
        }

        # analyze this "line" for comments, procedure declarations,
        # and procedure calls
        if {[string index $line 0] == "\#"} {
            append output "<font color=\"$::comment_color\">[sanitize $line]</font>"
            append comment_block [string range $line 1 end]
            # check to see if this is a file-level comment
            if [regexp -- {\A\/\/\#.*\/\/\#\Z} [string trim $comment_block]] {
                add_file_annotation \
                    [string range [string trim $comment_block] 3 end-3]
                set comment_block ""
            } else {
                append comment_block "\n"
            }
            set line ""
        } elseif {[regexp -- {\A(proc\s+)(\S+)(.*)} $line foo decl procname line]} {
            # using $::line_number below may lead to incorrect numbers
            # because $output may have newlines buffered within.  thus
            # first flush $output (and hence increment ::line_number).
            write_and_update $dest $output
            set output "<strong><a name=\"${procname}_${::line_number}\">$decl<a href=\"$annotname\#$procname\">$procname</a></a></strong>"
            # additionally, if this is the /last/ declaration for the
            # function add a normal <a name> anchor
            set procrecord [lookup_procrecord $procname $basename]
            if {[lindex $procrecord 1] == $::line_number} {
                append output "<a name=\"${procname}\"></a>"
            }
            if [catch {set proc_args [flatten_args [lindex $line 0]]}] {
                tcldoc_file_warning "Malformed syntax for procedure arguments, skipping"
            } else {
                add_proc_annotation [string trim $comment_block] $procname \
                    $proc_args $::line_number
            }
            set comment_block ""
        } elseif {[regexp -- {\A([:A-Za-z_]\S*)(.*)} $line foo decl line]} {
            # check to see if line is a call to a previously declared
            # procedure
            set procrecord [lookup_procrecord $decl $basename]
            if {$procrecord != {}} {
                foreach {procdest procline} $procrecord {}
                set procdest \
                        "[file join [path_lookup $procdest] $procdest].html"
                set procid ${decl}_${procline}
                # add to the procedure call table this call
                set callcount 1
                if [info exists ::call_table($decl)] {
                    foreach call $::call_table($decl) {
                        if {[string match "$basename*" $call]} {
                            incr callcount
                        }
                    }
                }
                lappend ::call_table($decl) "${basename}($callcount)"
                append output "<a name=\"${decl}($callcount)\"><a href=\"$procdest#$procid\">$decl</a></a>"
            } else {
                # not a known command
                append output $decl
            }
            set comment_block ""
        } else {
            set comment_block ""
        }

        write_and_update $dest $output

        # the rest of the line may have more info, so recurse upon any
        # part which appears to be a sublist or a subcommand.  for
        # everything else write it to the destination
        while {$line != ""} {
            set c [string index $line 0]
            if {$c == "\{" || $c == "\["} {
                # find the matching brace/bracket
                set found_matching 0
                set subgroup "$c"
                set line [string range $line 1 end]
                while {!$found_matching && $line != ""} {
                    if [regexp -- {\A([^\}\]]*[\}\]])(.*)} $line foo s line] {
                        append subgroup $s
                    } else {
                        break
                    }
                    set found_matching [info complete $subgroup]
                }
                if $found_matching {
                    puts -nonewline $dest [string index $subgroup 0]
                    scan_recursively $dest [string range $subgroup 1 end-1] $basename $annotname
                    puts -nonewline $dest [string index $subgroup end]
                } else {
                    tcldoc_file_error "Unmatched $c"
                }
            } elseif [regexp -- {\A((\\.|[^\[\{])+)(.*)} $line foo match foo2 line] {
                write_and_update $dest [sanitize $match]
            }
        }
        
        # consume the next character from the buffer
        write_and_update $dest [string index $buffer 0]
        set buffer [string range $buffer 1 end]
    }
}


# Writes to channel <code>$dest</code> the contents of
# <code>$output</code>.  Updates the global <code>::line_number</code>
# to keep track of how many lines have been written; hopefully this
# number is the same as the lines read from the source file.
#
# @param dest channel to write <code>$output</code>
# @param output data to write
proc write_and_update {dest output} {
    # write to the destination file and update the line counter
    if {$output != ""} {
        puts -nonewline $dest $output
        incr ::line_number -1
        foreach x [split $output "\n"] {
            incr ::line_number
        }
    }
}

# Given a procedure name, look up within the procedure table for its
# declaration.  In case of ambiguity as to which function declaration
# to use prefer to use the last one declared within $basename.
# Otherwise just use the first one listed (and hope for the best!).
# Returns a two element list containing the Tcl source filename and
# line number where procedure was declared.  If the procedure is not
# declared at all return an empty list.
#
# @param procname procedure name to look up
# @param basename preferred file to use
# @return if entry found a 2-ple procedure record, else an empty list
proc lookup_procrecord {procname basename} {
    set procrecord ""
    if [info exists ::proc_table($procname)] {
        foreach pr $::proc_table($procname) {
            if {[lindex $pr 0] == $basename} {
                set procrecord $pr
            }
        }
        if {$procrecord == ""} {
            set procrecord [lindex $::proc_table($procname) 0]
        }
    }
    return $procrecord
}

# Given some text, replaces potentially dangerous characters with
# their HTML character code.  Returns the new string afterwards.
#
# @param s string to sanitize
# @return an HTML-friendly version of <code>$s</code>
proc sanitize {s} {
    regsub -all {\&} $s {\&amp;} s
    regsub -all {\<} $s {\&lt;} s
    regsub -all {\>} $s {\&gt;} s
    regsub -all {\"} $s {\&#34;} s
    return $s
}

# Outputs a common header for HTML-ized Tcl files.
#
# @param dest I/O channel to write HTML header
# @param basename Tcl source filename, sans any directory paths
# @param title HTML title to use for generated file
proc write_header {dest basename title} {
    puts $dest "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">
<!-- Generated by TclDoc $::TCLDOC_VERSION -->
<html>
<head><title>$title</title></head>
<body bgcolor=\"$::page_bg_color\">"
    if {$::header != ""} {
        puts $dest "$::header\n<hr>"
    }
    if {!$::hide_navbar} {
        puts $dest "<font size=\"-2\">"
        if {$::overview_file != ""} {
            puts -nonewline $dest "<a href=\"[file tail $::overview_file]\">Overview</a> | "
        }
        puts $dest "Index by:  <a href=\"index_main.html#$basename\">file name</a> |
<a href=\"index_main.html#byprocname\">procedure name</a> |
<a href=\"index_main.html#bycall\">procedure call</a> |
<a href=\"index_annot_full.html\">annotation</a>
</font>
<hr>"
    }
}

# Output a common header for HTML-ized Tcl files.  This same footer is
# also used for index_main.html.
#
# @param dest I/O channel to write HTML footer
proc write_footer {dest} {
    if {!$::hide_navbar} {
        puts $dest "<hr>\n<font size=\"-2\">"
        if {$::overview_file != ""} {
            puts -nonewline $dest "<a href=\"[file tail $::overview_file]\">Overview</a> | "
        }
        puts $dest "Index by:  <a href=\"index_main.html#byfilename\">file name</a> |
<a href=\"index_main.html#byprocname\">procedure name</a> |
<a href=\"index_main.html#bycall\">procedure call</a> |
<a href=\"index_annot_full.html\">annotation</a><br>
<cite>File generated $::date.</cite>
</font>"
    }
    if {$::footer != ""} {
        puts $dest "<hr>\n$::footer"
    }
    puts $dest "</body>\n</html>"
}

######################################################################
# functions used when writing the index files

# Write the index of filenames.  Alphabetically list all source files
# along with procedures declared within.  Add hyperlinks from those
# procedure names to the line where they are declared.  Also write to
# the main index a similar list.
#
# @param mainindex I/O channel of index_main.html
proc write_index_byfile {mainindex} {
    print_status "  building index by file name"
    set fileindexname [file join $::dest_dir "index_file.html"]
    if [catch {open $fileindexname w} fileindex] {
        tcldoc_error "  Unable to create index_file.html" $::IO_ERROR
    }
    write_index_header $fileindex $::dest_dir \
        "<strong>file name</strong> |
<a href=\"index_proc.html\" target=\"sidebar\">procedure name</a> |
<a href=\"index_call.html\" target=\"sidebar\">procedure call</a> |
<a href=\"index_annot.html\" target=\"sidebar\">annotation</a>" "file name"

    # iterate through all files.  add an entry along with all
    # functions declared within that file.
    foreach filename $::all_files {
        set basename [file tail $filename]
        set dir [path_lookup $basename]
        set htmlname "[file join $dir $basename].html"
        set txtname "[file join $dir $basename].txt"
        set annothtmlname "[file join $dir $basename]-annot.html"
        
        puts $mainindex "<p>
<dt><strong><a name=\"$basename\"><a href=\"$htmlname\">$basename</a></a></strong>"
        puts $mainindex "(<a href=\"$annothtmlname\">annotations</a> | <a href=\"$txtname\">original source</a>)"
        puts $fileindex "<dt><strong><a name=\"$basename\"><a href=\"$htmlname\" target=\"main\">$basename</a></a></strong>"
        puts $fileindex "<font size=\"-2\">(<a href=\"$annothtmlname\" target=\"main\">annotations</a> | <a href=\"$txtname\" target=\"main\">original</a>)</font>"
        
        # list all procedure declarations in that file, if any
        if [info exists ::file_table($basename)] {
            foreach procrecord [lsort -dictionary -index 0 $::file_table($basename)] {
                foreach {procname line} $procrecord {}
                set procid ${procname}_${line}
                puts $mainindex "<dd><a href=\"$htmlname#$procid\">$procname</a>"
                puts $fileindex "<dd><a href=\"$htmlname#$procid\" target=\"main\">$procname</a>"
            }
        }
    }
    
    write_index_footer $fileindex
    close $fileindex
}

# Write the index of procedures.  Alphabetically list all procedure
# declarations; if a procedure is declared multiple times list all of
# them.  Add hyperlinks from those procedure names to the line where
# they are declared.  Also write to the main index a similar list.
#
# @param mainindex I/O channel of index_main.html
proc write_index_byproc {mainindex} {
    print_status "  building index by procedure name"
    set procindexname [file join $::dest_dir "index_proc.html"]
    if [catch {open $procindexname w} procindex] {
        tcldoc_error "  Unable to create index_proc.html" $::IO_ERROR
    }
    write_index_header $procindex $::dest_dir \
        "<a href=\"index_file.html\" target=\"sidebar\">file name</a> |
<strong>procedure name</strong> |
<a href=\"index_call.html\" target=\"sidebar\">procedure call</a> |
<a href=\"index_annot.html\" target\"sidebar\">annotation</a>" "procedure name"

    # iterate through all procedures declarations.  add an entry along
    # with the Tcl file and line containing its declaration
    set firstlet "\0"
    foreach procname [lsort -dictionary [array names ::proc_table]] {
        set filenames $::proc_table($procname)
        if {[string compare -nocase -length 1 $firstlet $procname] < 0} {
            set firstlet [string tolower [string index $procname 0]]
            puts $mainindex "<dt><strong>$firstlet</strong>"
        }
        puts -nonewline $mainindex "<dd>$procname: "

        # if the procedure has multiple declarations (for some
        # reason), list each instance on a separate line within
        # index_proc.html.  for index_main.html, however, give the
        # procedure name followed by a comma separated list of source
        # files
        if {[llength $filenames] == 1} {
            # only one declaration; set a link straight from the procedure name
            foreach {filename line} [lindex $filenames 0] {}
            set procid ${procname}_${line}
            set htmlname "[file join [path_lookup $filename] $filename].html"
            puts $procindex "<a href=\"$htmlname#$procid\" target=\"main\">$procname</a><br>"
        } else {
            puts $procindex "$procname:"
            foreach filerecord [lsort -dictionary -index 0 $filenames] {
                foreach {filename line} $filerecord {}
                set procid ${procname}_${line}
                set htmlname "[file join [path_lookup $filename] $filename].html"
                puts $procindex " <li><a href=\"$htmlname#$procid\" target=\"main\">$filename</a>"
            }
            puts $procindex "<br>"
        }
        set filelist ""
        foreach filerecord [lsort -dictionary -index 0 $filenames] {
            foreach {filename line} $filerecord {}
            set procid ${procname}_${line}
            set htmlname "[file join [path_lookup $filename] $filename].html"
            lappend filelist "<a href=\"$htmlname#$procid\">$filename</a>"
        }
        puts $mainindex [join $filelist ", "]
    }
    write_index_footer $procindex
    close $procindex
}

# Write the index of procedure calls.  Alphabetically list every
# procedure that is called.  Add hyperlinks to the line where that
# call is made.  Also write to the main index a similar list.
#
# @param mainindex I/O channel of index_main.html
proc write_index_bycall {mainindex} {
    print_status "  building index by procedure call"
    set callindexname [file join $::dest_dir "index_call.html"]
    if [catch {open $callindexname w} callindex] {
        tcldoc_error "  Unable to create index_call.html" $::IO_ERROR
    }
    write_index_header $callindex $::dest_dir \
        "<a href=\"index_file.html\" target=\"sidebar\">file name</a> |
<a href=\"index_proc.html\" target=\"sidebar\">procedure name</a> |
<strong>procedure call</strong> |
<a href=\"index_annot.html\" target=\"sidebar\">annotation</a>" "procedure call"

    # iterate through all procedure calls.  add an entry along with
    # the Tcl file(s) that make that call.
    set firstlet "\0"
    foreach procname [lsort -dictionary [array names ::call_table]] {
        set calls $::call_table($procname)
        if {[string compare -nocase -length 1 $firstlet $procname] < 0} {
            set firstlet [string tolower [string range $procname 0 0]]
            puts $mainindex "<dt><strong>$firstlet</strong>"
        }
        puts -nonewline $mainindex "<dd><strong>$procname:</strong> "
        puts -nonewline $callindex "<dt><strong>$procname</strong>"
        set filelist_main ""
        set filelist_call ""
        foreach call [lsort -dictionary $calls] {
            # each entry in ::call_table is of the form `foo(x)' where
            # foo is the source file and (x) is an identifier
            regexp {(.*)(\(\d+\)$)} $call foo filename callnum
            set callid "${procname}$callnum"
            set htmlname "[file join [path_lookup $filename] $filename].html"
            lappend filelist_main "<a href=\"$htmlname#$callid\">$call</a>"
            lappend filelist_call "<dd><a href=\"$htmlname#$callid\" target=\"main\">$call</a>"
        }
        puts $mainindex [join $filelist_main ", "]
        puts $callindex [join $filelist_call ", "]
    }
    write_index_footer $callindex
    close $callindex
}


# Write two indices of all declared procedures and source files.  The
# big index (index_annot_full.html) alphabetizes everything and
# displays a one-line summary along with a hyperlink to the item.  The
# smaller index, index_annot.html, has just the item names and
# hyperlinks.
proc write_index_annotations {} {
    print_status "Building annotation indices..."
    set annotindexname [file join $::dest_dir "index_annot.html"]
    if [catch {open $annotindexname w} annotindex] {
        tcldoc_error "  Unable to create index_annot.html." $::IO_ERROR
    }
    set annotfullindexname [file join $::dest_dir "index_annot_full.html"]
    if [catch {open $annotfullindexname w} annotfullindex] {
        tcldoc_error "  Unable to create index_annot_full.html." $::IO_ERROR
    }
    write_index_header $annotindex $::dest_dir \
        "<a href=\"index_file.html\" target=\"sidebar\">file name</a> |
<a href=\"index_proc.html\" target=\"sidebar\">procedure name</a> |
<a href=\"index_call.html\" target=\"sidebar\">procedure call</a> |
<strong>annotation</strong>" "annotation"
    write_index_header $annotfullindex $::dest_dir \
        "<a href=\"index_main.html#byfilename\">file name</a> |
<a href=\"index_main.html#byprocname\">procedure name</a> |
<a href=\"index_main.html#bycall\">procedure call</a> |
<strong>annotation</strong>" ""

    foreach c { : 1 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z } {
        if $::toc_table($c) {
            puts $annotfullindex "<a href=\"#$c\">$c</a>"
        } else {
            puts $annotfullindex $c
        }
    }

    puts $annotfullindex "<h1>Index of annotations</h1>\n<dl>"
    set firstlet " "
    foreach summary_name [lsort -dictionary [array names ::summary_table]] {
        set new_firstlet [string toupper [string index $summary_name 0]]
        if {$new_firstlet != ":"} {
            if {[string compare $new_firstlet "A"] < 0} {
                set new_firstlet "1"
            } elseif {[string compare $new_firstlet "Z"] > 0} {
                set new_firstlet "Z"
            }
        }
        if {[string compare $firstlet $new_firstlet] < 0} {
            set firstlet $new_firstlet
            puts $annotfullindex "<dt><h3><a name=\"$firstlet\">$firstlet</a></h3>"
        }
        set summary_entry_list $::summary_table($summary_name)
        # if more than one entry with the same summary_name, then show
        # each one using a bulleted list
        if {[llength $summary_entry_list] > 1} {
            puts $annotindex "<dt>$summary_name:"
        }
        foreach summary_entry $summary_entry_list {
            # a summary is:  target, args, source, description, type, new/old
            foreach {target args source desc type} $summary_entry {}
            set markup_start ""
            set markup_stop ""
            if {$type == "file"} {
                set markup_start "<strong>"
                set markup_stop "</strong>"
            }
            if {[llength $summary_entry_list] == 1} {
                puts $annotindex "<dt>$markup_start<a href=\"$target\" target=\"main\">$summary_name</a>$markup_stop"
            } else {
                puts $annotindex "<li>$markup_start<a href=\"$target\" target=\"main\">$source</a>$markup_stop"
            }
            puts $annotfullindex "<dt>$markup_start<a href=\"$target\"\>$summary_name</a>$markup_stop $args - "
            if {$type == "file"} {
                if {$source == ""} {
                    puts $annotfullindex "Tcl source code"
                } else {
                    puts $annotfullindex "file found in <code>$source</code>"
                }
            } else {
                puts $annotfullindex "<a href=\"[file join [path_lookup $source] $source]-annot.html\">$source</a>"
            }
            puts $annotfullindex "<dd>$desc"
        }
    }
    write_index_footer $annotindex
    puts $annotfullindex "</dl>"
    write_footer $annotfullindex
    close $annotindex
    close $annotfullindex
}

# Writes the overall index.html that defines the frames.  If an
# overview file was specified (with <code>--overview</code>) then have
# the index load the overview; otherwise just load index_main.html.
proc write_index_master {} {
    print_status "Building index.html..."
    set indexname [file join $::dest_dir "index.html"]
    if [catch {open $indexname w} index] {
        tcldoc_error "  Unable to create index.html." $::IO_ERROR
    }
    puts $index "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">
<!-- Generated by TclDoc $::TCLDOC_VERSION -->
<html>
<head>
<title>$::title</title>
</head>
<frameset rows=\"*\" cols=\"25%, *\">
<frame src=\"index_file.html\" name=\"sidebar\">"
    if {$::overview_file != ""} {
        puts $index "<frame src=\"[file tail $::overview_file]\" name=\"main\">
<noframes>
You want to go <a href=\"[file tail $::overview_file]\">here</a>."
    } else {
        puts $index "<frame src=\"index_main.html\" name=\"main\">
<noframes>
You want to go <a href=\"index_main.html\">here</a>."
    }
    puts $index "</noframes>
</frameset>
</html>"

    close $index
}

# Outputs a common header for the various generated index files.
#
# @param dest I/O channel to write HTML footer
# @param page_title HTML title to use for generated file
# @param index_line HTML source to print for the <code>Index by:</code> line
# @param page_header an optional title to put at the top of the page
proc write_index_header {dest page_title index_line {page_header ""}} {
    puts $dest "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">
<!-- Generated by TclDoc $::TCLDOC_VERSION -->
<html>
<head>
<title>$page_title</title>
</head>
<body bgcolor=\"$::page_bg_color\">
<script type=\"text/javascript\">
<!--
if (top == self) {
    location.href=\"index.html\"
}
//-->
</script>"
    if {$::header != ""} {
        puts $dest "$::header\n<hr>"
    }
    puts $dest "<font size=\"-2\">"
    if {$::overview_file != ""} {
        puts -nonewline $dest "<a href=\"[file tail $::overview_file]\" target=\"main\">Overview</a> | "
    }
    puts $dest "Index by:  $index_line</font><br>\n<hr>"
    if {$page_header != ""} {
        puts $dest "<strong>Index by $page_header:</strong>\n<dl>"
    }
}

# Outputs a common footer for the various generated index files.
#
# @param dest I/O channel to write HTML footer
proc write_index_footer {dest} {
    puts $dest "</dl>
<p>
<hr>
<font size=\"-2\"><cite>Index generated $::date.</cite></font>
</body>
</html>"
}

######################################################################
# File Utilities

# Glob recursively across a directory and its subdirectory for all
# files matching a list of extensions.  Return all matches as a flat
# list.
#
# @param dir root directory to scan
# @param exts list of extension (e.g., <code>*.tcl</code>) to search
# @return list of matching files
proc glob_all {dir exts} {
    set all_files [glob -nocomplain -directory $dir *]
    set retval ""
    foreach ext $exts {
        set foundfiles [glob -nocomplain -directory $dir $ext]
        foreach file $foundfiles {
            if {[file isfile $file] && [file readable $file]} {
                lappend retval $file
            }
        }
    }
    foreach file $all_files {
        if [file isdirectory $file] {
            set retval [concat $retval [glob_all [file join $dir $file] $exts]]
        }
    }
    return $retval
}

# Compares the last part of a filename (i.e., sans directory paths).
# Returns-1, 0, 1 if respectively <code>$a</code> occurs lexically
# before, with, or after <code>$b</code>.
#
# @param a first file to compare
# @param b second file to compare
# @return -1, 0, or 1
proc filecomp {a b} {
    return [string compare -nocase [file tail $a] [file tail $b]]
}

# Given a filename returns the location of where its TclDoc'ed files
# are located.  The path may not necessarily be the same as
# <code>$::dest_dir</code>, especially if the file is being imported
# from elsewhere by way of <code>--import</code>.
#
# @param orig_file filename to find
# @return path to where TclDoc wrote its file
proc path_lookup {orig_file} {
    if [info exists ::path_table($orig_file)] {
        return $::path_table($orig_file);
    } else {
        return "."
    }
}


######################################################################
# miscellaneous TclDoc utilities

# If running in verbose mode print to standard output its arguments.
# Otherwise do nothing.
#
# @param args any valid string suitable to be passed to <code>puts</code>
proc print_status {args} {
    if $::verbose {
        eval puts $args
    }
}

# Given an arbitrary length list (such as the one supplying arguments
# to a procedure declaration) remove excess spaces between arguments.
# This is very similar to Lisp's flatten function.
#
# @param x list to flatten
# @return a flattend list
proc flatten_args {x} {
    if {![info complete $x]} {
        tcldoc_error "ERROR: Attempting to flatten $x"
    }    
    set new_list ""
    foreach elem $x {
        if {[llength $elem] > 1} {
            lappend new_list [flatten_args $elem]
        } else {
            lappend new_list $elem
        }
    }
    return $new_list
}

# Adds an entry to the global summary table.  The entry will
# eventually be written to the global summary indices.
#
# @param entry brief entry name
# @param target for file entries the HTML version of the file; for
# procedures the file containing its declaration
# @param arguments for procedures a list or arguments to it; ignored
# for files
# @param source source Tcl file for the entry
# @param description a one line summary describing the entry
# @param type type of entry; currently just <code>file</code> and
# <code>proc</code> are understood.
# @see write_index_annotations
proc add_summary {entry target arguments source description type} {
    lappend ::summary_table($entry) \
        [list $target $arguments $source $description $type]    
    set firstchar [string toupper [string index $entry 0]]
    if {$firstchar == ":"} {
        incr ::toc_table(:)
    } elseif {[string compare $firstchar "A"] < 0} {
        incr ::toc_table(1)
    } elseif {[string compare $firstchar "Z"] > 0} {
        incr ::toc_table(Z)
    } else {
        incr ::toc_table($firstchar)
    }
}

# Called whenever TclDoc found a problem with a file, particularly
# something that it could not parse.  Print to standard error the
# message along with the source file and line number if verbose
# reporting was enabled.
#
# @param message message to display
proc tcldoc_file_warning {message} {
    if $::verbose {
        puts stderr "$message (file $::current_file, line $::line_number)"
    }
}

# Called to abort whenever TclDoc discovers a problem with a
# particular input file.  Print to standard error the message along
# with the source file and line number where that error occured.
# Finally abort program.
#
# @param message message to display
proc tcldoc_file_error {message} {
    puts stderr "$message (file $::current_file, line $::line_number)"
    exit $::SYNTAX_ERROR
}

# Called to abort TclDoc upon all other errors.  Print to standard
# error the error message then abort TclDoc.
#
# @param message message to display
# @param returnvalue exit code
proc tcldoc_error {message {returnvalue -1}} {
    puts stderr $message
    exit $returnvalue
}

# Retrives a parameter from the options list.  If no parameter exists
# then abort with an error very reminisicent of C's
# <code>getopt</code> function; otherwise increment
# <code>param_num</code> by one.
#
# @param param_list list of parameters from the command line
# @param param_num index into <code>param_list</code> to retrieve
# @param param_name name of the parameter, used when reporting an error
# @return the <code>$param_num</code>'th element into <code>$param_list</code>
proc get_param {param_list param_num param_name} {
    upvar $param_num pn
    incr pn
    if {$pn >= [llength $param_list]} {
        tcldoc_error "TclDoc: option requires an argument -- $param_name" $::PARAM_ERROR
    }
    return [lindex $param_list $pn]
}

# Print TclDoc's usage to a channel.
#
# @param chan I/O channel to print usage documentation
proc print_tcldoc_help {chan} {
    puts $chan "TclDoc: a Tcl API Documentation Generator
Usage:  TclDoc \[options\] DESTDIR SRC \[SRC...\]
  DESTDIR           direction to which write generated files
  SRC               Tcl source code file to parse

General Options:
  -h, --help           print this help message and quit
  -v, --verbose        be verbose while generating files
  -f, --force          don't prompt before overwriting files in DESTDIR
  --version            show TclDoc version and quit
  --                   marks end of options

Overall File Generation Options:
  --overview FILE      use FILE as an overview page
  --doc-files NAME     copy NAME (file or directory) verbatim to DESTDIR\[*\]
  --dont-copy          don't copy original source files to DESTDIR

Individual File Generation Options:
  --title TITLE        use TITLE for the main index.html page
  --header HTML        use HTML text in the header
  --footer HTML        use HTML text in the footer
  --hide-paths         prevent showing path names on annotated pages
  --no-navbar          disable navigation bar at top and bottom of pages
  --date FORMAT        write time stamp using FORMAT \(see \[clock format\]\)
  --comment COLOR      hex COLOR for comments \(default \"208020\"\)
  --page-bg COLOR      hex COLOR for page backgrounds \(default \"ffffff\"\)
  --table-bg COLOR     hex COLOR for table annotations \(default \"ccccff\"\)

Import/Export Options: \(not implemented yet\)
  --import FILE        \[*\]
  --include FILE       \[*\]
  --export FILE      
  --export-loc NEWDIR
\[*\] Multiple invocations of this option allowed."
}


######################################################################
# other TclDoc functions

# Parse the command line and set global options.
#
# @param argv list of options from the command line
proc tcldoc_args {argv} {
    set argvp 0
    set ::verbose 0
    set ::force_overwrite 0
    set ::overview_file ""
    set ::dont_copy_files 0
    set ::doc_dir ""
    set ::title "TclDoc Documentation"
    set ::header ""
    set ::footer ""
    set ::hide_paths 0
    set ::hide_navbar 0
    set ::import_file ""
    set ::export_file ""
    set ::export_dir ""
    set date_format "%Y-%m-%d at %H:%M"
    set ::comment_color "\#208020";     # a pale green color
    set ::page_bg_color "\#ffffff";     # pearly white
    set ::table_bg_color "\#ccccff";    # pale blue
    while {$argvp < [llength $argv]} {
        set arg [lindex $argv $argvp]
        switch -- $arg {
            "-h" - "--help"      { print_tcldoc_help stdout; exit }
            "-v" - "--verbose"   { set ::verbose 1}
            "-f" - "--force"     { set ::force_overwrite 1 }
            "--version" { puts "TclDoc version $::TCLDOC_VERSION"; exit }
            "--overview" {
                set ::overview_file [get_param $argv argvp "overview"]
            }
            "--doc-files" { lappend ::doc_dir [get_param $argv argvp "doc-files"] }
            "--dont-copy" { set ::dont_copy_files 1 }
            "--title"     { set ::title [get_param $argv argvp "title"] }
            "--header"    { set ::header [get_param $argv argvp "header"] }
            "--footer"    { set ::footer [get_param $argv argvp "footer"] }
            "--hide-paths" { set ::hide_paths 1 }
            "--no-navbar"  { set ::hide_navbar 1 }
            "--date"       { set date_format [get_param $argv argvp "date"] }
            "--comment" {
                set ::comment_color "\#[get_param $argv argvp "comment"]"
            }
            "--page-bg" {
                set ::page_bg_color "\#[get_param $argv argvp "page-bg"]"
            }
            "--table-bg" {
                set ::table_bg_color "\#[get_param $argv argvp "table-bg"]"
            }
            "--import"   { set ::import_file [get_param $argv argvp "import"] }
            "--export"   { set ::export_file [get_param $argv argvp "export"] }
            "--export-loc" {
                set ::export_dir [get_param $argv argvp "export-loc"]
            }
            "--"           { incr argvp; break }
            default {
                if {[string index $arg 0] != "-"} {
                    break
                } else {
                    puts stderr "TclDoc: unknown option $arg"
                    print_tcldoc_help stderr
                    exit $::PARAM_ERROR
                }
            }
        }
        incr argvp
    }
    if {$argvp + 2 > [llength $argv]} {
        puts stderr "Must specify a destination directory and at least one source file."
        print_tcldoc_help stderr
        exit $::PARAM_ERROR
    }
    set ::date [clock format [clock seconds] -format $date_format]
    set ::dest_dir [lindex $argv $argvp]
    set ::srcs [lrange $argv [expr {$argvp + 1}] end]
    if {$::export_dir != ""} {
        set ::export_dir [file join [pwd] $::dest_dir]
    }    
}

# Actually run TclDoc across requested files and directories.  Scan
# them and generate HTML markup versions.  Scan file and procedure
# comments to build the annotated files.  Cross-reference procedure
# calls with the declarations.  Finally write indices to everything.
proc tcldoc_main {} {
    # first build a list of all tcl scripts which are defined as
    # those with filenames *.tcl or *.tsh
    set ::todo_files ""
    foreach src $::srcs {
        if [file isfile $src] {
            lappend ::todo_files $src
        } else {
            foreach srcf [glob_all $src {*.tcl *.tsh}] {
                lappend ::todo_files $srcf
            }
        }
    }
    set ::todo_files [lsort -ascii -command filecomp $::todo_files]
    set ::all_files [lsort -ascii -command filecomp [concat $::all_files $::todo_files]]

    # open each file and scan for procedure declarations
    foreach filename $::todo_files {
        declaration_scan $filename
    }
    
    # rescan each file, this time identifying procedure calls and
    # other markups.  write both its HTML version and its annotated
    # version.
    foreach filename $::todo_files {
        deep_scan $filename
        set basename [file tail $filename]
        set newtxtname "[file join [path_lookup $basename] $basename].txt"
        if {!$::dont_copy_files} {
            file copy -force $filename [file join $::dest_dir $newtxtname]
        }        
    }

    # begin constructing the main index page, which is the combination
    # of all three major indices (by file name, by procedure name, by
    # procedure call) group onto a single page.
    print_status "Building index_main.html..."
    set mainindexname [file join $::dest_dir "index_main.html"]
    if [catch {open $mainindexname w} mainindex] {
        tcldoc_error "  Unable to create index_main.html." $::IO_ERROR
    }
    write_index_header $mainindex $::dest_dir \
        "<a href=\"index_main.html#byfilename\">file name</a> |
<a href=\"index_main.html#byprocname\">procedure name</a> |
<a href=\"index_main.html#bycall\">procedure call</a> |
<a href=\"index_annot_full.html\">annotation</a>"
    puts $mainindex "<h1>TclDoc of <em>$::dest_dir</em></h1>"

    # construct the index by file name page
    puts $mainindex "<h2>Index by <a name=\"byfilename\">file name</a>:</h2>\n<dl>"
    write_index_byfile $mainindex

    # construct the index by procedure name
    puts $mainindex "
</dl>
<hr>
<h2>Index by <a name=\"byprocname\">procedure name</a>:</h2>
<dl>"
    write_index_byproc $mainindex

    # construct the index by procedure call
    puts $mainindex "
</dl>
<hr>
<h2>Index by <a name=\"bycall\">procedure call</a>:</h2>
<dl>"
    write_index_bycall $mainindex

    puts $mainindex "</dl>"
    write_footer $mainindex
    close $mainindex

    # build the annotations index
    write_index_annotations
    
    # finally, build the master index_main.html page
    write_index_master
}

######################################################################
# start of main script

set ::IO_ERROR 1
set ::SYNTAX_ERROR 2
set ::PARAM_ERROR 3
set ::GRAMMAR_ERROR 4

tcldoc_args $argv
initialize_tables
prepare_destination
tcldoc_main
write_export_file

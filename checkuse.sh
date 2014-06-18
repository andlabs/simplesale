awk '
/^extern/ {
	n = $3
	if ($2 == "const")
		n = $4
	gsub(/\(.*$/, "", n)
	gsub(/^\*/, "", n)
	re = "[^a-zA-Z0-9_]" n "[^a-zA-Z0-9_]"
	printf "echo `grep -l \"%s\" *.c | wc -l` \"     \" %s\n", re, n
}
' < simplesale.h | bash

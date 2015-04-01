// 25 january 2015
package main

import (
	"fmt"
	"os"
	"encoding/xml"
	"text/template"
)

// TODO really cheat like this? or just use x->priv.abc?
const outTemplate = `// generated by tools/ui2cdecl.go; do not edit
typedef struct {{.TypeName}} {{.TypeName}};
struct {{.TypeName}} {
	struct {{.TypeName}}Priv *priv;
	struct {{.TypeName}}Priv _priv;			// cheating
{{range .Members}}	{{.Type}} *{{.Name}};
{{end}}};
static {{.TypeName}} *make{{.TypeName}}FromUIFile(void)
{
	{{.TypeName}} *instance;
	GError *err = NULL;
	GtkBuilder *builder;

	instance = g_new({{.TypeName}}, 1);
	instance->priv = &(instance->_priv);
	builder = gtk_builder_new();
	if (gtk_builder_add_from_resource(builder, "{{.GResourcePath}}", &err) == 0)
		g_error("error loading {{.GResourcePath}}: %s", err->message);
{{$typename := .TypeName}}{{range .Members}}	instance->{{.Name}} = {{.CastTo}}(gtk_builder_get_object(builder, "{{.Name}}"));
	if (instance->{{.Name}} == NULL)
		g_error("error getting {{$typename}} member {{.Name}} (exact error unspecified)");
{{if .Keep}}	g_object_ref(instance->{{.Name}});
{{end}}{{end}}	g_object_unref(builder);
	return instance;
}
`

type member struct {
	Name	string
	Type		string
	CastTo	string
	Keep		bool
}

type outParams struct {
	TypeName		string
	GResourcePath		string
	Members			[]*member
}

var knownTypes = map[string]string{
	"GtkTreeSelection":		"GTK_TREE_SELECTION",
	"GtkTextBuffer":		"GTK_TEXT_BUFFER",
	"GtkTextTag":			"GTK_TEXT_TAG",
	"GtkTextTagTable":		"GTK_TEXT_TAG_TABLE",
	"GtkAdjustment":		"GTK_ADJUSTMENT",
}

type uiProp struct {
	Name	string		`xml:"name,attr"`
	Value	string		`xml:",chardata"`
}

type uiChild struct {
	Objects	[]uiObject		`xml:"object"`
}

type uiObject struct {
	Class	string		`xml:"class,attr"`
	ID		string		`xml:"id,attr"`
	Props	[]uiProp		`xml:"property"`
	Children	[]uiChild		`xml:"child"`
}

type uiInterface struct {
	Objects	[]uiObject		`xml:"object"`
}

func die(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format, args...)
	fmt.Fprintf(os.Stderr, "\n")
	os.Exit(1)
}

func main() {
	var iface uiInterface
	var f func([]uiObject)

	if len(os.Args) != 4 {
		die("usage: %s uifile cfile grespath", os.Args[0])
	}

	uifile, err := os.Open(os.Args[1])
	if err != nil {
		die("error opening .ui file %s: %v", os.Args[1], err)
	}
	defer uifile.Close()

	cfile, err := os.Create(os.Args[2])
	if err != nil {
		die("error creating .c file %s: %v", os.Args[2], err)
	}
	defer cfile.Close()

	grespath := os.Args[3]
	members := make([]*member, 0, 50)

	uireader := xml.NewDecoder(uifile)
	err = uireader.Decode(&iface)
	if err != nil {
		die("error parsing .ui file: %v", err)
	}

	typename := ""
	f = func(objs []uiObject) {
		for _, o := range objs {
			found := false
			for name, conv := range knownTypes {
				if o.Class == name {
					members = append(members, &member{
						Name:	o.ID,
						Type:	name,
						CastTo:	conv,
					})
					found = true
					break
				}
			}
			if !found {
				members = append(members, &member{
					Name:	o.ID,
					Type:	"GtkWidget",
					CastTo:	"GTK_WIDGET",
					// needed because the only class that GtkBuilder tries to preserve itself is GtkWindow; any widget not in a GtkWindow that we don't explicitly ref is lost when the builder is destroyed
					Keep:	(o.ID == "main" && o.Class != "GtkWindow"),
				})
			}
			if o.ID == "main" {
				for _, p := range o.Props {
					if p.Name == "name" {
						typename = p.Value
						break
					}
				}
			}
			for _, c := range o.Children {
				f(c.Objects)
			}
		}
	}
	f(iface.Objects)

	if typename == "" {
//TODO		die("error: type name not in .ui file")
	}

	t, err := template.New("outTemplate").Parse(outTemplate)
	if err != nil {
		die("error parsing .c output template: %v", err)
	}
	err = t.Execute(cfile, &outParams{
		TypeName:		typename,
		GResourcePath:	grespath,
		Members:			members,
	})
	if err != nil {
		die("error generating .c output file: %v", err)
	}
}

/*
{{ct.className}}::{{ct.className}}()
{
//	deserializeActor(this, __type());
//	init();
}
*/

void {{ct.className}}::__init()
{    
    {% if ct.primary %}
    {% else %}
    ActorsXmlLoader::setSize(this, rs.getResAnim("{{ct.member}}"));
    {% endif %}
    {% for member in members %}
    {{member.name}} = safeCast<{{member.class_type.className}}*>(_getDescendant("{{member.name}}"));    
    {% endfor %}
};


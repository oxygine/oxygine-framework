class {{ct.className}}: public {{ct.parent.className}}
{
public:
	FACTORY_ACTOR({{ct.className}}, "{{ct.className}}");

	void __init();

    {% for member in members %}
    {{ member.class_type.spClassName }} {{ member.name}};
    {% endfor %}    
protected:
};



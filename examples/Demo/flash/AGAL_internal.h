#ifndef AS3_IMPL // class interfaces
#pragma GCC diagnostic ignored "-Wdiv-by-zero" push
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class VM;
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class Expression;
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class ExpressionParser;
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class AGALVar;
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class AGALPreAssembler;
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
class PerspectiveMatrix3D;
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
class AGALMiniAssembler;
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
class AGALMacroAssembler;
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class VM : public AS3_NS::Object {
protected:
	explicit VM(unsigned id) : AS3_NS::Object(id) {}
public:
	AS3_BASE_DEF(VM, kNull);
	class internal {
		AS3_IPROP_SET_GET_DEF(AS3_NS::Array, stack);
		AS3_IPROP_SET_GET_DEF(AS3_NS::flash::utils::Dictionary, vars);
	public:
		static Class getClosure();
		class instance : public AS3_NS::Object::internal::instance {
		public:
			AS3_IPROP(stack);
			AS3_IPROP(vars);
			AS3_METH_DEF(void, pushIf, ());
			AS3_METH_DEF(void, popEndif, ());
			AS3_METH_DEF(void, setIf, ( double));
			AS3_METH_DEF(bool, ifWasTrue, ());
			AS3_METH_DEF(bool, ifIsTrue, ());
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	AS3_CONSTR_DEF(VM, ());
};
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class Expression : public AS3_NS::Object {
protected:
	explicit Expression(unsigned id) : AS3_NS::Object(id) {}
public:
	AS3_BASE_DEF(Expression, kNull);
	class internal {
	public:
		static Class getClosure();
		class instance : public AS3_NS::Object::internal::instance {
		public:
			AS3_METH_DEF(void, print, ( int));
			AS3_METH_DEF(void, exec, ( AS3_NS::com::adobe::utils::macro::VM));
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	AS3_CONSTR_DEF(Expression, ());
};
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class ExpressionParser : public AS3_NS::Object {
protected:
	explicit ExpressionParser(unsigned id) : AS3_NS::Object(id) {}
public:
	AS3_BASE_DEF(ExpressionParser, kNull);
	class internal {
	public:
		static Class getClosure();
		class instance : public AS3_NS::Object::internal::instance {
		public:
			AS3_METH_DEF(AS3_NS::com::adobe::utils::macro::Expression, parse, ( AS3_NS::var, AS3_NS::String));
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	AS3_CONSTR_DEF(ExpressionParser, ());
};
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class AGALVar : public AS3_NS::Object {
protected:
	explicit AGALVar(unsigned id) : AS3_NS::Object(id) {}
public:
	AS3_BASE_DEF(AGALVar, kNull);
	class internal {
		AS3_IPROP_SET_GET_DEF(AS3_NS::String, name);
		AS3_IPROP_SET_GET_DEF(AS3_NS::String, target);
		AS3_IPROP_SET_GET_DEF(double, w);
		AS3_IPROP_SET_GET_DEF(double, x);
		AS3_IPROP_SET_GET_DEF(double, y);
		AS3_IPROP_SET_GET_DEF(double, z);
	public:
		static Class getClosure();
		class instance : public AS3_NS::Object::internal::instance {
		public:
			AS3_IPROP(name);
			AS3_IPROP(target);
			AS3_IPROP(w);
			AS3_IPROP(x);
			AS3_IPROP(y);
			AS3_IPROP(z);
			AS3_METH_DEF(bool, isConstant, ());
			AS3_METH_DEF(AS3_NS::String, toString, ());
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	AS3_CONSTR_DEF(AGALVar, ());
};
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
namespace macro {
class AGALPreAssembler : public AS3_NS::Object {
protected:
	explicit AGALPreAssembler(unsigned id) : AS3_NS::Object(id) {}
public:
	AS3_BASE_DEF(AGALPreAssembler, kNull);
	class internal {
	public:
		static Class getClosure();
		AS3_PROP_GET_DEF(bool, TRACE_AST);
		AS3_PROP_GET_DEF(bool, TRACE_PREPROC);
		AS3_PROP_GET_DEF(bool, TRACE_VM);
		class instance : public AS3_NS::Object::internal::instance {
		public:
			AS3_METH_DEF(bool, processLine, ( AS3_NS::var, AS3_NS::String));
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	static AS3_PROP(TRACE_AST);
	static AS3_PROP(TRACE_PREPROC);
	static AS3_PROP(TRACE_VM);
	AS3_CONSTR_DEF(AGALPreAssembler, ());
};
} //macro
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
class PerspectiveMatrix3D : public AS3_NS::flash::geom::Matrix3D {
protected:
	explicit PerspectiveMatrix3D(unsigned id) : AS3_NS::flash::geom::Matrix3D(id) {}
public:
	AS3_BASE_DEF(PerspectiveMatrix3D, kNull);
	class internal {
	public:
		static Class getClosure();
		class instance : public AS3_NS::flash::geom::Matrix3D::internal::instance {
		public:
			AS3_METH_DEF(void, lookAtLH, ( AS3_NS::flash::geom::Vector3D, AS3_NS::flash::geom::Vector3D, AS3_NS::flash::geom::Vector3D));
			AS3_METH_DEF(void, lookAtRH, ( AS3_NS::flash::geom::Vector3D, AS3_NS::flash::geom::Vector3D, AS3_NS::flash::geom::Vector3D));
			AS3_METH_DEF(void, perspectiveLH, ( double, double, double, double));
			AS3_METH_DEF(void, perspectiveRH, ( double, double, double, double));
			AS3_METH_DEF(void, perspectiveFieldOfViewLH, ( double, double, double, double));
			AS3_METH_DEF(void, perspectiveFieldOfViewRH, ( double, double, double, double));
			AS3_METH_DEF(void, perspectiveOffCenterLH, ( double, double, double, double, double, double));
			AS3_METH_DEF(void, perspectiveOffCenterRH, ( double, double, double, double, double, double));
			AS3_METH_DEF(void, orthoLH, ( double, double, double, double));
			AS3_METH_DEF(void, orthoRH, ( double, double, double, double));
			AS3_METH_DEF(void, orthoOffCenterLH, ( double, double, double, double, double, double));
			AS3_METH_DEF(void, orthoOffCenterRH, ( double, double, double, double, double, double));
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	AS3_CONSTR_DEF(PerspectiveMatrix3D, ( AS3_NS::var = *(AS3_NS::var*)&AS3_NS::internal::_null));
};
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
class AGALMiniAssembler : public AS3_NS::Object {
protected:
	explicit AGALMiniAssembler(unsigned id) : AS3_NS::Object(id) {}
public:
	AS3_BASE_DEF(AGALMiniAssembler, kNull);
	class internal {
		AS3_IPROP_GET_DEF(AS3_NS::flash::utils::ByteArray, agalcode);
		AS3_IPROP_GET_DEF(AS3_NS::String, error);
		AS3_IPROP_SET_GET_DEF(bool, verbose);
	public:
		static Class getClosure();
		class instance : public AS3_NS::Object::internal::instance {
		public:
			AS3_IPROP(agalcode);
			AS3_IPROP(error);
			AS3_IPROP(verbose);
			AS3_METH_DEF(AS3_NS::flash::utils::ByteArray, assemble, ( AS3_NS::String, AS3_NS::String));
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	AS3_CONSTR_DEF(AGALMiniAssembler, ( bool = false));
};
} //utils
} //adobe
} //com
namespace com {
namespace adobe {
namespace utils {
class AGALMacroAssembler : public AS3_NS::com::adobe::utils::AGALMiniAssembler {
protected:
	explicit AGALMacroAssembler(unsigned id) : AS3_NS::com::adobe::utils::AGALMiniAssembler(id) {}
public:
	AS3_BASE_DEF(AGALMacroAssembler, kNull);
	class internal {
		AS3_IPROP_SET_GET_DEF(AS3_NS::flash::utils::Dictionary, aliases);
		AS3_IPROP_SET_GET_DEF(AS3_NS::String, asmCode);
		AS3_IPROP_SET_GET_DEF(bool, profile);
		AS3_IPROP_SET_GET_DEF(AS3_NS::String, profileTrace);
	public:
		static Class getClosure();
		AS3_PROP_GET_DEF(AS3_NS::RegExp, IDENTIFIER);
		AS3_PROP_GET_DEF(AS3_NS::RegExp, NUMBER);
		AS3_PROP_GET_DEF(AS3_NS::RegExp, OPERATOR);
		AS3_PROP_GET_DEF(AS3_NS::RegExp, PREPROC);
		AS3_PROP_GET_DEF(AS3_NS::RegExp, SEPERATOR);
		AS3_PROP_GET_DEF(AS3_NS::String, STDLIB);
		AS3_PROP_GET_DEF(AS3_NS::RegExp, TOKEN);
		class instance : public AS3_NS::com::adobe::utils::AGALMiniAssembler::internal::instance {
		public:
			AS3_IPROP(aliases);
			AS3_IPROP(asmCode);
			AS3_IPROP(profile);
			AS3_IPROP(profileTrace);
			AS3_METH_DEF(AS3_NS::flash::utils::ByteArray, assemble, ( AS3_NS::String, AS3_NS::String));
		};
	};
	internal::instance *operator->() { return (internal::instance *)this; }
	static AS3_PROP(IDENTIFIER);
	static AS3_PROP(NUMBER);
	static AS3_PROP(OPERATOR);
	static AS3_PROP(PREPROC);
	static AS3_PROP(SEPERATOR);
	static AS3_PROP(STDLIB);
	static AS3_PROP(TOKEN);
	static AS3_METH_DEF(AS3_NS::String, joinTokens, ( AS3_NS::var));
	AS3_CONSTR_DEF(AGALMacroAssembler, ( bool = false));
};
} //utils
} //adobe
} //com
#pragma GCC diagnostic ignored "-Wdiv-by-zero" pop
#else // class implementations
AS3_BASE_IMPL(com::adobe::utils::macro::, VM, "com.adobe.utils.macro.", "VM");
AS3_IPROP_SET_GET_IMPL(AS3_NS::Array, stack, com::adobe::utils::macro::, VM, "stack");
AS3_IPROP_SET_GET_IMPL(AS3_NS::flash::utils::Dictionary, vars, com::adobe::utils::macro::, VM, "vars");
AS3_IMETH_IMPL(void, com::adobe::utils::macro::, VM, pushIf, (), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.macro.VM; var thiz:VM = as3_id2rcv[%1].val; try { thiz.pushIf(); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 4)
AS3_IMETH_IMPL(void, com::adobe::utils::macro::, VM, popEndif, (), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.macro.VM; var thiz:VM = as3_id2rcv[%1].val; try { thiz.popEndif(); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 4)
AS3_IMETH_IMPL(void, com::adobe::utils::macro::, VM, setIf, ( double arg0), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.macro.VM; var thiz:VM = as3_id2rcv[%1].val; try { thiz.setIf(%2); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 12)
AS3_IMETH_IMPL(bool, com::adobe::utils::macro::, VM, ifWasTrue, (), {
	unsigned eid;
	bool res;
	inline_as3 ("import com.adobe.utils.macro.VM; var thiz:VM = as3_id2rcv[%2].val; try { %1 = thiz.ifWasTrue(); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(res) : "r"(_id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	return res;
}, 4)
AS3_IMETH_IMPL(bool, com::adobe::utils::macro::, VM, ifIsTrue, (), {
	unsigned eid;
	bool res;
	inline_as3 ("import com.adobe.utils.macro.VM; var thiz:VM = as3_id2rcv[%2].val; try { %1 = thiz.ifIsTrue(); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(res) : "r"(_id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	return res;
}, 4)
AS3_CONSTR_IMPL(com::adobe::utils::macro::, VM, (), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.VM; try { %1 = as3_valAcquireId(new VM()); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::macro::VM res;
	AS3_GET_ID(res) = id;
	return res;
}, 4)
AS3_BASE_IMPL(com::adobe::utils::macro::, Expression, "com.adobe.utils.macro.", "Expression");
AS3_IMETH_IMPL(void, com::adobe::utils::macro::, Expression, print, ( int arg0), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.macro.Expression; var thiz:Expression = as3_id2rcv[%1].val; try { thiz.print(%2); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 8)
AS3_IMETH_IMPL(void, com::adobe::utils::macro::, Expression, exec, ( AS3_NS::com::adobe::utils::macro::VM arg0), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.macro.Expression; var thiz:Expression = as3_id2rcv[%1].val; try { thiz.exec(as3_id2rcv[%2].val); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(AS3_GET_ID(arg0)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 8)
AS3_CONSTR_IMPL(com::adobe::utils::macro::, Expression, (), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.Expression; try { %1 = as3_valAcquireId(new Expression()); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::macro::Expression res;
	AS3_GET_ID(res) = id;
	return res;
}, 4)
AS3_BASE_IMPL(com::adobe::utils::macro::, ExpressionParser, "com.adobe.utils.macro.", "ExpressionParser");
AS3_IMETH_IMPL(AS3_NS::com::adobe::utils::macro::Expression, com::adobe::utils::macro::, ExpressionParser, parse, ( AS3_NS::var arg0, AS3_NS::String arg1), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.ExpressionParser; var thiz:ExpressionParser = as3_id2rcv[%2].val; try { %1 = as3_valAcquireId(thiz.parse(as3_id2rcv[%3].val, as3_id2rcv[%4].val)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(_id), "r"(AS3_GET_ID(arg0)), "r"(AS3_GET_ID(arg1)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::macro::Expression res;
	AS3_GET_ID(res) = id;
	return res;
}, 16)
AS3_CONSTR_IMPL(com::adobe::utils::macro::, ExpressionParser, (), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.ExpressionParser; try { %1 = as3_valAcquireId(new ExpressionParser()); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::macro::ExpressionParser res;
	AS3_GET_ID(res) = id;
	return res;
}, 4)
AS3_BASE_IMPL(com::adobe::utils::macro::, AGALVar, "com.adobe.utils.macro.", "AGALVar");
AS3_IPROP_SET_GET_IMPL(AS3_NS::String, name, com::adobe::utils::macro::, AGALVar, "name");
AS3_IPROP_SET_GET_IMPL(AS3_NS::String, target, com::adobe::utils::macro::, AGALVar, "target");
AS3_IPROP_SET_GET_SIMPL(double, w, com::adobe::utils::macro::, AGALVar, "w");
AS3_IPROP_SET_GET_SIMPL(double, x, com::adobe::utils::macro::, AGALVar, "x");
AS3_IPROP_SET_GET_SIMPL(double, y, com::adobe::utils::macro::, AGALVar, "y");
AS3_IPROP_SET_GET_SIMPL(double, z, com::adobe::utils::macro::, AGALVar, "z");
AS3_IMETH_IMPL(bool, com::adobe::utils::macro::, AGALVar, isConstant, (), {
	unsigned eid;
	bool res;
	inline_as3 ("import com.adobe.utils.macro.AGALVar; var thiz:AGALVar = as3_id2rcv[%2].val; try { %1 = thiz.isConstant(); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(res) : "r"(_id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	return res;
}, 4)
AS3_IMETH_IMPL(AS3_NS::String, com::adobe::utils::macro::, AGALVar, toString, (), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.AGALVar; var thiz:AGALVar = as3_id2rcv[%2].val; try { %1 = as3_valAcquireId(thiz.toString()); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(_id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::String res;
	AS3_GET_ID(res) = id;
	return res;
}, 8)
AS3_CONSTR_IMPL(com::adobe::utils::macro::, AGALVar, (), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.AGALVar; try { %1 = as3_valAcquireId(new AGALVar()); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::macro::AGALVar res;
	AS3_GET_ID(res) = id;
	return res;
}, 4)
AS3_BASE_IMPL(com::adobe::utils::macro::, AGALPreAssembler, "com.adobe.utils.macro.", "AGALPreAssembler");
AS3_PROP_GET_SIMPL(bool, TRACE_AST, com::adobe::utils::macro::AGALPreAssembler, "com.adobe.utils.macro.AGALPreAssembler.TRACE_AST", "com.adobe.utils.macro.AGALPreAssembler");
AS3_PROP_GET_SIMPL(bool, TRACE_PREPROC, com::adobe::utils::macro::AGALPreAssembler, "com.adobe.utils.macro.AGALPreAssembler.TRACE_PREPROC", "com.adobe.utils.macro.AGALPreAssembler");
AS3_PROP_GET_SIMPL(bool, TRACE_VM, com::adobe::utils::macro::AGALPreAssembler, "com.adobe.utils.macro.AGALPreAssembler.TRACE_VM", "com.adobe.utils.macro.AGALPreAssembler");
AS3_IMETH_IMPL(bool, com::adobe::utils::macro::, AGALPreAssembler, processLine, ( AS3_NS::var arg0, AS3_NS::String arg1), {
	unsigned eid;
	bool res;
	inline_as3 ("import com.adobe.utils.macro.AGALPreAssembler; var thiz:AGALPreAssembler = as3_id2rcv[%2].val; try { %1 = thiz.processLine(as3_id2rcv[%3].val, as3_id2rcv[%4].val); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(res) : "r"(_id), "r"(AS3_GET_ID(arg0)), "r"(AS3_GET_ID(arg1)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	return res;
}, 12)
AS3_CONSTR_IMPL(com::adobe::utils::macro::, AGALPreAssembler, (), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.macro.AGALPreAssembler; try { %1 = as3_valAcquireId(new AGALPreAssembler()); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::macro::AGALPreAssembler res;
	AS3_GET_ID(res) = id;
	return res;
}, 4)
AS3_BASE_IMPL(com::adobe::utils::, PerspectiveMatrix3D, "com.adobe.utils.", "PerspectiveMatrix3D");
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, lookAtLH, ( AS3_NS::flash::geom::Vector3D arg0, AS3_NS::flash::geom::Vector3D arg1, AS3_NS::flash::geom::Vector3D arg2), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.lookAtLH(as3_id2rcv[%2].val, as3_id2rcv[%3].val, as3_id2rcv[%4].val); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(AS3_GET_ID(arg0)), "r"(AS3_GET_ID(arg1)), "r"(AS3_GET_ID(arg2)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 16)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, lookAtRH, ( AS3_NS::flash::geom::Vector3D arg0, AS3_NS::flash::geom::Vector3D arg1, AS3_NS::flash::geom::Vector3D arg2), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.lookAtRH(as3_id2rcv[%2].val, as3_id2rcv[%3].val, as3_id2rcv[%4].val); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(AS3_GET_ID(arg0)), "r"(AS3_GET_ID(arg1)), "r"(AS3_GET_ID(arg2)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 16)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, perspectiveLH, ( double arg0, double arg1, double arg2, double arg3), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.perspectiveLH(%2, %3, %4, %5); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 36)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, perspectiveRH, ( double arg0, double arg1, double arg2, double arg3), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.perspectiveRH(%2, %3, %4, %5); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 36)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, perspectiveFieldOfViewLH, ( double arg0, double arg1, double arg2, double arg3), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.perspectiveFieldOfViewLH(%2, %3, %4, %5); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 36)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, perspectiveFieldOfViewRH, ( double arg0, double arg1, double arg2, double arg3), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.perspectiveFieldOfViewRH(%2, %3, %4, %5); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 36)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, perspectiveOffCenterLH, ( double arg0, double arg1, double arg2, double arg3, double arg4, double arg5), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.perspectiveOffCenterLH(%2, %3, %4, %5, %6, %7); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 52)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, perspectiveOffCenterRH, ( double arg0, double arg1, double arg2, double arg3, double arg4, double arg5), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.perspectiveOffCenterRH(%2, %3, %4, %5, %6, %7); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 52)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, orthoLH, ( double arg0, double arg1, double arg2, double arg3), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.orthoLH(%2, %3, %4, %5); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 36)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, orthoRH, ( double arg0, double arg1, double arg2, double arg3), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.orthoRH(%2, %3, %4, %5); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 36)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, orthoOffCenterLH, ( double arg0, double arg1, double arg2, double arg3, double arg4, double arg5), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.orthoOffCenterLH(%2, %3, %4, %5, %6, %7); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 52)
AS3_IMETH_IMPL(void, com::adobe::utils::, PerspectiveMatrix3D, orthoOffCenterRH, ( double arg0, double arg1, double arg2, double arg3, double arg4, double arg5), {
	unsigned eid;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; var thiz:PerspectiveMatrix3D = as3_id2rcv[%1].val; try { thiz.orthoOffCenterRH(%2, %3, %4, %5, %6, %7); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid) : "r"(_id), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
}, 52)
AS3_CONSTR_IMPL(com::adobe::utils::, PerspectiveMatrix3D, ( AS3_NS::var arg0), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.PerspectiveMatrix3D; try { %1 = as3_valAcquireId(new PerspectiveMatrix3D(as3_id2rcv[%2].val)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(AS3_GET_ID(arg0)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::PerspectiveMatrix3D res;
	AS3_GET_ID(res) = id;
	return res;
}, 8)
AS3_BASE_IMPL(com::adobe::utils::, AGALMiniAssembler, "com.adobe.utils.", "AGALMiniAssembler");
AS3_IPROP_GET_IMPL(AS3_NS::flash::utils::ByteArray, agalcode, com::adobe::utils::, AGALMiniAssembler, "agalcode");
AS3_IPROP_GET_IMPL(AS3_NS::String, error, com::adobe::utils::, AGALMiniAssembler, "error");
AS3_IPROP_SET_GET_SIMPL(bool, verbose, com::adobe::utils::, AGALMiniAssembler, "verbose");
AS3_IMETH_IMPL(AS3_NS::flash::utils::ByteArray, com::adobe::utils::, AGALMiniAssembler, assemble, ( AS3_NS::String arg0, AS3_NS::String arg1), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.AGALMiniAssembler; var thiz:AGALMiniAssembler = as3_id2rcv[%2].val; try { %1 = as3_valAcquireId(thiz.assemble(as3_id2rcv[%3].val, as3_id2rcv[%4].val)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(_id), "r"(AS3_GET_ID(arg0)), "r"(AS3_GET_ID(arg1)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::flash::utils::ByteArray res;
	AS3_GET_ID(res) = id;
	return res;
}, 16)
AS3_CONSTR_IMPL(com::adobe::utils::, AGALMiniAssembler, ( bool arg0), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.AGALMiniAssembler; try { %1 = as3_valAcquireId(new AGALMiniAssembler(%2)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(arg0));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::AGALMiniAssembler res;
	AS3_GET_ID(res) = id;
	return res;
}, 8)
AS3_BASE_IMPL(com::adobe::utils::, AGALMacroAssembler, "com.adobe.utils.", "AGALMacroAssembler");
AS3_IPROP_SET_GET_IMPL(AS3_NS::flash::utils::Dictionary, aliases, com::adobe::utils::, AGALMacroAssembler, "aliases");
AS3_IPROP_SET_GET_IMPL(AS3_NS::String, asmCode, com::adobe::utils::, AGALMacroAssembler, "asmCode");
AS3_IPROP_SET_GET_SIMPL(bool, profile, com::adobe::utils::, AGALMacroAssembler, "profile");
AS3_IPROP_SET_GET_IMPL(AS3_NS::String, profileTrace, com::adobe::utils::, AGALMacroAssembler, "profileTrace");
AS3_PROP_GET_IMPL(AS3_NS::RegExp, IDENTIFIER, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.IDENTIFIER", "com.adobe.utils.AGALMacroAssembler");
AS3_PROP_GET_IMPL(AS3_NS::RegExp, NUMBER, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.NUMBER", "com.adobe.utils.AGALMacroAssembler");
AS3_PROP_GET_IMPL(AS3_NS::RegExp, OPERATOR, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.OPERATOR", "com.adobe.utils.AGALMacroAssembler");
AS3_PROP_GET_IMPL(AS3_NS::RegExp, PREPROC, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.PREPROC", "com.adobe.utils.AGALMacroAssembler");
AS3_PROP_GET_IMPL(AS3_NS::RegExp, SEPERATOR, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.SEPERATOR", "com.adobe.utils.AGALMacroAssembler");
AS3_PROP_GET_IMPL(AS3_NS::String, STDLIB, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.STDLIB", "com.adobe.utils.AGALMacroAssembler");
AS3_PROP_GET_IMPL(AS3_NS::RegExp, TOKEN, com::adobe::utils::AGALMacroAssembler, "com.adobe.utils.AGALMacroAssembler.TOKEN", "com.adobe.utils.AGALMacroAssembler");
AS3_IMETH_IMPL(AS3_NS::flash::utils::ByteArray, com::adobe::utils::, AGALMacroAssembler, assemble, ( AS3_NS::String arg0, AS3_NS::String arg1), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.AGALMacroAssembler; var thiz:AGALMacroAssembler = as3_id2rcv[%2].val; try { %1 = as3_valAcquireId(thiz.assemble(as3_id2rcv[%3].val, as3_id2rcv[%4].val)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(_id), "r"(AS3_GET_ID(arg0)), "r"(AS3_GET_ID(arg1)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::flash::utils::ByteArray res;
	AS3_GET_ID(res) = id;
	return res;
}, 16)
AS3_METH_IMPL(AS3_NS::String, com::adobe::utils::AGALMacroAssembler::joinTokens, ( AS3_NS::var arg0), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.AGALMacroAssembler; try { %1 = as3_valAcquireId(AGALMacroAssembler.joinTokens(as3_id2rcv[%2].val)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(AS3_GET_ID(arg0)));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::String res;
	AS3_GET_ID(res) = id;
	return res;
}, 8)
AS3_CONSTR_IMPL(com::adobe::utils::, AGALMacroAssembler, ( bool arg0), {
	unsigned eid;
	unsigned id;
	inline_as3 ("import com.adobe.utils.AGALMacroAssembler; try { %1 = as3_valAcquireId(new AGALMacroAssembler(%2)); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : "=r"(eid), "=r"(id) : "r"(arg0));
	if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }
	AS3_NS::com::adobe::utils::AGALMacroAssembler res;
	AS3_GET_ID(res) = id;
	return res;
}, 8)
#endif // classes 
#ifndef AS3_IMPL // function interfaces
#else // function implementations
#endif // functions
#ifndef AS3_IMPL // property interfaces
#else // property implementations
#endif // properties

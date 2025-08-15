#pragma once

// 委托名，参数
DECLARE_DELEGATE(FSingleDelegateWithoutParam);
DECLARE_DELEGATE_OneParam(FSingleDelegateWithOneParam, FString);
DECLARE_DELEGATE_TwoParams(FSingleDelegateWithTwoParam, FString, int32);
// 返回值，委托名，参数
DECLARE_DELEGATE_RetVal_OneParam(FString, FStringDelegateWithReturn, int32);

class CallBackTarget
{
public:
	void SDNoParamCallBack();
	void SDOneParamCallBack(FString Param);
	void SDTwoParamCallBack(FString Param, int32 Value);

	UFUNCTION()
	void SDOneParamCallback_UFunction(FString str);

	UFUNCTION()
	FString SDOneParamWithRetValue_UFunction(int32 a);

	// 静态成员函数
	static void StaticMemberFunc(FString str, int32 value) { }
};

//全局静态函数(静态非成员函数)
static void StaticFunc(FString str, int32 value) { }

class delegation
{
public:
	void Test()
	{
		CallBackTarget* Target = new CallBackTarget();

		/* NOTE: BindUObject 参数讲解
		 * InUserObject:被绑定委托的对象
		 * InFunc:被绑定的回调函数,注意,InFunc是InUserObject的成员函数
		 *
		 * 注意理解!! InUserObject是InFunc的拥有者,传入这两个参数的目的是为了确认
		 * 委托要绑定 谁(InUserObject)的函数(InFunc)
		 */
		FSingleDelegateWithoutParam SDNoParam;
		SDNoParam.BindUObject(Target, &CallBackTarget::SDNoParamCallBack);

		/* NOTE: BindUFunction 参数讲解:
		 * InUserObject:被绑定委托的对象
		 * InFunctionName:被绑定的回调函数名.
		 * 注意!! ue4之所以仅根据函数名就能正确绑定回调函数,是通过ue4的反射机制,
		 * 这就要求被绑定的函数必须要被UFUNCTION()宏包住,否则无法正确的找到对应的函数
		 * 导致绑定失败.
		 * 
		 * 注意理解!! InUserObject是InFunctionName的拥有者,传入这两个参数的目的是为了确认
		 * 委托要绑定 谁(InUserObject)的函数(InFunctionName)
		 */
		FSingleDelegateWithOneParam SDOneParam;
		SDOneParam.BindUFunction(Target, "SDOneParamCallback_UFunction");

		/* NOTE: BindStatic 可以绑定静态成员函数，也可以绑定全局函数（静态非成员函数）
		 * 需要保证绑定的函数和委托的参数一致
		 */
		FSingleDelegateWithTwoParam SDTwoParam;
		SDTwoParam.BindStatic(&CallBackTarget::StaticMemberFunc);		// 绑定静态成员函数
		SDTwoParam.BindStatic(&StaticFunc);								// 绑定全局函数

		/* NOTE: 绑定lambda表达式
		 */
		// SDOneParam.BindLambda(
		// 	[](FString str){ }
		// );

		/* NOTE: CreateUObject 返回一个绑定完成的委托对象，用来初始化SDOneParam
		 */
		FStringDelegateWithReturn SDOneParamWithRetValue = FStringDelegateWithReturn::CreateUObject(Target, &CallBackTarget::SDOneParamWithRetValue_UFunction);

		/* NOTE: 委托触发
		 * 单播委托对象使用ExecuteIfBound()触发回调
		 * 可以使用IsBound()判断是否已经绑定
		 */
		SDNoParam.ExecuteIfBound();
		SDOneParam.ExecuteIfBound("test single delegate");
		SDTwoParam.ExecuteIfBound("test single delegate", 100);
		FString rtvstr = SDOneParamWithRetValue.ExecuteIfBound(123);

		delete Target;
	}
};

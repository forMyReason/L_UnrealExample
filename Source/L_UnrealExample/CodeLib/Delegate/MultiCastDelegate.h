#pragma once

// 委托名、参数
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiCastDelegateWithOneParam, FString);

class CallBackTarget
{
	void FunctionForAddObject(FString str);

	UFUNCTION()
	void FunctionForAddUFunction(FString str);

	static void MemberStaticFunc(FString str) { }
};

static void GlobalStaticFunc(FString str) { }

class MultiCastDelegate
{
public:
	void Test()
	{
		// 创建多播对象
		FMultiCastDelegateWithOneParam MDOneParam;

		CallBackTarget* Target = new CallBackTarget();

		// Delegate Handle
		FDelegateHandle Handle;

		/* 多播绑定回调函数以Add开头 */
		Handle = MDOneParam.AddUObject(Target, &CallBackTarget::FunctionForAddObject);
		Handle = MDOneParam.AddUFunction(Target, FName("FunctionForAddUFunction"));
		Handle = MDOneParam.AddStatic(GlobalStaticFunc);
		Handle = MDOneParam.AddStatic(&CallBackTarget::MemberStaticFunc);
		Handle = MDOneParam.AddLambda(
			[](FString str)
			{
				
			});
		
		// 触发回调函数，可以使用IsBound()判断是否已经绑定
		if (MDOneParam.IsBound)
		{
			MDOneParam.Broadcast("Multicast delegate trigger");
		}

		// 解绑所有绑定的函数
		MDOneParam.Clear();
		// 移除单个回调函数(Handle是绑定函数回调时返回的 FDelegateHandle)
		MDOneParam.Remove(Handle);

		// RemoveAll(InUserObject),假如多播委托绑定了某一个对象的多个函数作为回调
		// 可以传入对象，把该对象的所有被绑定的成员函数解绑
		// CallbackTarget* Target=new CallbackTarget();
		MDOneParam.RemoveAll(Target);
	}
};

#pragma once

class CallBackTarget
{
public:
	void FunctionForAddObject(FString str);

	UFUNCTION()
	void FunctionForAddUFunction(FString str);
	
	static void MemberStaticFunc(FString str) { }
};

static void GlobalStaticFunc(FString str) { }

class MultiCastDelegateEvent
{
public:
	/*
	 * OwingType:拥有此Event的类,本例中使用本类:MultiCastDelegateEvent
	 * EventName:事件名称
	 * ParamType:参数列表
	 */
	DECLARE_EVENT_OneParam(MultiCastDelegateEvent, MyDelegateEvent, FString);

	// NOTE:公开对Event对象的访问接口
	MyDelegateEvent& OnEventTrigger() { return DelegateEvent; }

	// ....
	
private:
	MyDelegateEvent DelegateEvent;

public:
	void Test()
	{
		CallBackTarget* Target = new CallBackTarget();

		// 绑定
		FDelegateHandle Handle;
		Handle = OnEventTrigger().AddUObject(Target, &CallBackTarget::FunctionForAddObject);
		Handle = OnEventTrigger().AddUFunction(Target, "FunctionForAddUFunction");
		Handle = OnEventTrigger().AddStatic(&CallBackTarget::MemberStaticFunc);
		Handle = OnEventTrigger().AddStatic(GlobalStaticFunc);
		Handle = OnEventTrigger().AddLambda(
			[](FString str)
			{
				//...
			});

		// 触发
		if (OnEventTrigger().IsBound())
		{
			OnEventTrigger().Broadcast("This is FString Params Input");
		}
	}
};
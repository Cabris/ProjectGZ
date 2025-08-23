#pragma once
// 這個檔不做宣告/定義，只提供「清單」巨集。
// 之後只改這裡，就能同步影響 .h 與 .cpp。
#define GZ_FOR_EACH_GAMEPLAY_TAG(OP)                             \
OP(Attribute_Primary_Health,     "Attribute.Primary.Health",     "Primary Health") \
OP(Attribute_Primary_MaxHealth,  "Attribute.Primary.MaxHealth",  "Primary Max Health") \
OP(InputTag_Move,                "InputTag.Move",                "WASD/左搖桿移動") \
OP(InputTag_Look,                "InputTag.Look",                "滑鼠/右搖桿視角") \
OP(InputTag_Jump,                "InputTag.Jump",                "跳躍") \
OP(InputTag_Sprint,              "InputTag.Sprint",              "衝刺") \
OP(InputTag_Crouch,              "InputTag.Crouch",              "蹲下") \
OP(InputTag_Aim,                 "InputTag.Aim",                 "瞄準") \
OP(InputTag_Strafe,              "InputTag.Strafe",              "橫移") \
OP(InputTag_Interaction,         "InputTag.Interaction",         "互動") \
OP(MessageTag_Effect_Applied,    "MessageTag.Effect.Applied",    "GE 套用訊息") \
OP(MessageTag_Inventory_Changed, "MessageTag.Inventory.Changed", "背包變更") \
OP(Item_Stack_Quantity,          "Item.Stack.Quantity",          "堆疊數量") \
OP(Item_Stack_Ammo,              "Item.Stack.Ammo",              "即時彈藥") \
OP(Item_Stack_Ammo_Spare,        "Item.Stack.Ammo.Spare",        "備用彈藥") \
OP(Action_Interaction_Collect,   "Action.Interaction.Collect",   "撿起") \
OP(Action_Interaction_OpenDoor,  "Action.Interaction.OpenDoor",  "開門") \
OP(Action_Attack_Ranged_Pistol,  "Action.Attack.Ranged.Pistol",  "手槍射擊") \
OP(Action_Attack_Ranged_Rifle,   "Action.Attack.Ranged.Rifle",   "步槍射擊") \
OP(State_Aiming,                 "State.Aiming",                 "正在瞄準") \
OP(State_Reloading,              "State.Reloading",              "裝填中") \
OP(State_Sprinting,              "State.Sprinting",              "衝刺中") \
OP(Cooldown_Generic,             "Cooldown.Generic",             "通用冷卻")

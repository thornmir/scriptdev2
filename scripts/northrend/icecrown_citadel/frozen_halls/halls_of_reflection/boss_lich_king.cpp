/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_lich_king
SD%Complete: 100%
SDComment:
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Halls of Reflection
EndScriptData */

#include "precompiled.h"
#include "def_halls.h"
#include "escort_ai.h"

enum
{
  SPELL_WINTER                       = 69780,
  SPELL_FURY_OF_FROSTMOURNE          = 70063,
  SPELL_SOUL_REAPER                  = 73797,
  SPELL_ICE_PRISON                   = 69708,
  SPELL_DARK_ARROW                   = 70194,
  SPELL_EMERGE_VISUAL                = 50142,
  SPELL_DESTROY_ICE_WALL_02          = 70224,
  SPELL_SILENCE                      = 69413,
  SPELL_LICH_KING_CAST               = 57561,
  SPELL_GNOUL_JUMP                   = 70150,
  SPELL_ABON_STRIKE                  = 40505,

  // summon spells
  SPELL_WITCH_DOCTOR                 = 69836,
  SPELL_SUMMON_ABOM                  = 69835,
  SPELL_RAISE_DEAD                   = 69818,

  /*SPELLS - Witch Doctor, 36941 */
  SPELL_COURSE_OF_DOOM               = 70144,
  SPELL_SHADOW_BOLT_VALLEY           = 70145,
  SPELL_SHADOW_BOLT                  = 70080,

  /*SPELLS - abomination, 37069 */
  SPELL_CLEAVE                       = 40505,

  /*SPELLS - raging ghoul, 36940 */
  SPELL_LEAP                         = 70150,

  SAY_LICH_KING_WALL_01              = -1594486,
  SAY_LICH_KING_WALL_02              = -1594491,
  SAY_LICH_KING_GNOUL                = -1594482,
  SAY_LICH_KING_ABON                 = -1594483,
  SAY_LICH_KING_WINTER               = -1594481,
  SAY_LICH_KING_END_DUN              = -1594504,
  SAY_LICH_KING_WIN                  = -1594485,
};

struct MANGOS_DLL_DECL boss_lich_king_hrAI : public npc_escortAI
{
   boss_lich_king_hrAI(Creature *pCreature) : npc_escortAI(pCreature)
   {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
   }

   ScriptedInstance* m_pInstance;
   uint32 Step;
   uint32 StepTimer;
   bool StartEscort;
   bool IceWall01;
   bool NonFight;
   bool Finish;

   void Reset()
   {
      if(!m_pInstance) return;
      NonFight    = false;
      StartEscort = false;
      m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(36942));
   }

   void JustDied(Unit* pKiller)
   {
   }

   void WaypointReached(uint32 i)
   {
        switch(i)
        {
            case 20:
                SetEscortPaused(true);
                Finish = true;
                DoCast(m_creature, SPELL_LICH_KING_CAST);
                m_pInstance->SetData(TYPE_LICH_KING, SPECIAL);
                DoScriptText(SAY_LICH_KING_END_DUN, m_creature);
                if(Creature* pLider = (m_creature->GetMap()->GetCreature( m_pInstance->GetData64(DATA_ESCAPE_LIDER))))
                {
                    pLider->CastSpell(pLider, SPELL_SILENCE, false);
                    pLider->AddSplineFlag(SPLINEFLAG_FLYING);
                    pLider->SendMonsterMove(pLider->GetPositionX(), pLider->GetPositionY(), pLider->GetPositionZ() + 4, SPLINETYPE_NORMAL , pLider->GetSplineFlags(), 3000); 
                }
                m_creature->SetActiveObjectState(false);
                break;
        }
   }

   void AttackStart(Unit* who) 
   {
      if (!m_pInstance) return;
      if (!who)        return;

     if (NonFight) return;

     if (m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS || who->GetTypeId() == TYPEID_PLAYER) return;

     npc_escortAI::AttackStart(who);
   }

    void SummonedCreatureJustDied(Creature* summoned)
    {
         if(!m_pInstance || !summoned) 
             return;
         m_pInstance->SetData(DATA_SUMMONS, 0);
    }

    void JustSummoned(Creature* summoned)
    {
         if(!m_pInstance || !summoned) return;

         summoned->SetPhaseMask(65535, true);
         summoned->SetInCombatWithZone();
         summoned->SetActiveObjectState(true);

         m_pInstance->SetData(DATA_SUMMONS, 1);

         if (Creature* pLider = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_ESCAPE_LIDER)))
         {
             summoned->GetMotionMaster()->MoveChase(pLider);
             summoned->AddThreat(pLider, 100.0f);
         }
    }

    void Wall01()
    {
      switch(Step)
      {
         case 0:
            SetEscortPaused(true);
            m_pInstance->SetData(DATA_SUMMONS, 3);
            DoScriptText(SAY_LICH_KING_WALL_01, m_creature);
            StepTimer = 2000;
            ++Step;
            break;
         case 1:
            StepTimer = 2000;
            ++Step;
            break;
         case 2:
            DoCast(m_creature, SPELL_RAISE_DEAD);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            StepTimer = 7000;
            ++Step;
            break;
         case 3:
            DoCast(m_creature, SPELL_WINTER);
            DoScriptText(SAY_LICH_KING_WINTER, m_creature);
            m_creature->SetSpeedRate(MOVE_WALK, 1.3f, true);
            StepTimer = 1000;
            ++Step;
            break;
         case 4:
            SetEscortPaused(false);
            StepTimer = 2000;
            ++Step;
            break;
         case 5:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            StepTimer = 100;
            ++Step;
            break;
       }
    }

    void Wall02()
    {
      switch(Step)
      {
          case 6:
            m_pInstance->SetData(DATA_SUMMONS, 3);
            SetEscortPaused(true);
            DoCast(m_creature, SPELL_RAISE_DEAD);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            StepTimer = 10000;
            ++Step;
            break;
          case 7:
            SetEscortPaused(false);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            StepTimer = 100;
            ++Step;
            break;
      }
   }

    void Wall03()
    {
      switch(Step)
      {
         case 8:
            m_pInstance->SetData(DATA_SUMMONS, 3);
            SetEscortPaused(true);
            DoCast(m_creature, SPELL_RAISE_DEAD);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            StepTimer = 10000;
            ++Step;
            break;
         case 9:
            SetEscortPaused(false);
            DoScriptText(SAY_LICH_KING_ABON, m_creature);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            m_pInstance->SetData(TYPE_ICE_WALL_03, DONE);
            StepTimer = 100;
            ++Step;
            break;
      }
    }

   void Wall04()
   {
      switch(Step)
      {
         case 10:
            m_pInstance->SetData(DATA_SUMMONS, 3);
            SetEscortPaused(true);
            DoCast(m_creature, SPELL_RAISE_DEAD);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            StepTimer = 10000;
            ++Step;
            break;
         case 11:
            SetEscortPaused(false);
            m_creature->SetSpeedRate(MOVE_WALK, 1.3f, true);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            StepTimer = 15000;
            ++Step;
            break;
         case 12:
            DoScriptText(SAY_LICH_KING_ABON, m_creature);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            StepTimer = 5000;
            ++Step;
            break;
      }
   }

   void UpdateEscortAI(const uint32 diff)
    {
      if(!m_pInstance) return;

      if(m_pInstance->GetData(TYPE_LICH_KING) != IN_PROGRESS)
      {
         if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
              return;

         DoMeleeAttackIfReady();
      }

      if(m_creature->isInCombat() && m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS)
         npc_escortAI::EnterEvadeMode();

      if(m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS && !StartEscort)
      {
         StartEscort = true;
         if(m_creature->HasAura(SPELL_ICE_PRISON))
            m_creature->RemoveAurasDueToSpell(SPELL_ICE_PRISON);
         if(m_creature->HasAura(SPELL_DARK_ARROW))
            m_creature->RemoveAurasDueToSpell(SPELL_DARK_ARROW);

         m_creature->SetActiveObjectState(true);

         NonFight = true;
         m_creature->AttackStop();
         m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
         m_creature->SetSpeedRate(MOVE_WALK, 2.7f, true);
         if (boss_lich_king_hrAI* pEscortAI = dynamic_cast<boss_lich_king_hrAI*>(m_creature->AI()))
             pEscortAI->Start();
         Step = 0;
         StepTimer = 100;
      }

        if (Creature* pLider = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_ESCAPE_LIDER)))
           if (pLider->IsWithinDistInMap(m_creature, 2.0f)) 
           {
               SetEscortPaused(true);
               DoScriptText(SAY_LICH_KING_WIN, m_creature);
               m_creature->CastSpell(m_creature, SPELL_FURY_OF_FROSTMOURNE, false);
               m_creature->DealDamage(pLider, pLider->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
               m_creature->NearTeleportTo(5572.077f, 2283.1f, 734.976f, 3.89f);
               m_pInstance->SetData(TYPE_LICH_KING, FAIL);
               m_creature->SetActiveObjectState(false);
               npc_escortAI::EnterEvadeMode();
           };

        if (m_pInstance->GetData(TYPE_ICE_WALL_01) == IN_PROGRESS)
        {
           if(StepTimer < diff)
              Wall01();
           else StepTimer -= diff;
        }
        else if (m_pInstance->GetData(TYPE_ICE_WALL_02) == IN_PROGRESS)
        {
           if(StepTimer < diff)
              Wall02();
           else StepTimer -= diff;
        }
        else if (m_pInstance->GetData(TYPE_ICE_WALL_03) == IN_PROGRESS)
        {
           if(StepTimer < diff)
              Wall03();
           else StepTimer -= diff;
        }
        else if (m_pInstance->GetData(TYPE_ICE_WALL_04) == IN_PROGRESS)
        {
           if(StepTimer < diff)
              Wall04();
           else StepTimer -= diff;
        }
        return;
    }
};

CreatureAI* GetAI_boss_lich_king_hr(Creature* pCreature)
{
    return new boss_lich_king_hrAI(pCreature);
}

struct MANGOS_DLL_DECL boss_lich_king_intro_horAI : public ScriptedAI
{
    boss_lich_king_intro_horAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
    }

    void JustDied(Unit* pKiller)
    {
    }

    void AttackStart(Unit* who) 
    {
         return;
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_boss_lich_king_intro_hor(Creature* pCreature)
{
    return new boss_lich_king_intro_horAI(pCreature);
}

struct MANGOS_DLL_DECL npc_undead_horAI : public BSWScriptedAI
{
    npc_undead_horAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void EnterEvadeMode()
    {
        if (!m_pInstance) 
            return;

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_LICH_KING) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        doCastAll(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_undead_hor(Creature* pCreature)
{
    return new npc_undead_horAI(pCreature);
}

void AddSC_boss_lich_king_hr()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lich_king_hr";
    newscript->GetAI = &GetAI_boss_lich_king_hr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lich_king_intro_hor";
    newscript->GetAI = &GetAI_boss_lich_king_intro_hor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_undead_hor";
    newscript->GetAI = &GetAI_npc_undead_hor;
    newscript->RegisterSelf();
}
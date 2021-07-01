/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polymorph.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:05:51 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:31:31 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Polymorph.hpp"

//constructor
Polymorph::Polymorph(void) : ASpell("Polymorph", "turned into a critter")
{}

Polymorph *
Polymorph::clone(void) const
{
	return (new Polymorph());	
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fireball.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:05:51 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:31:18 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fireball.hpp"

//constructor
Fireball::Fireball(void) : ASpell("Fireball", "burnt to a crisp")
{}

Fireball *
Fireball::clone(void) const
{
	return (new Fireball());	
}
